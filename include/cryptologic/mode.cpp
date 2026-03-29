#include "mode.h"
#include "DES/DES.h"
#include <cstdint>
#include <vector>
#include <string>
#include <cstring>
#include <thread>
#include <chrono>
#include <atomic>
#include <random>

void mode::secure_wipe(void* ptr, size_t len)
{
	static void* (*const volatile volatile_memset)(void*, int, size_t) = memset;

	volatile_memset(ptr, 0, len);
}

std::string mode::padding(std::string msg)
{	
	//copy msg
	std::string result = msg;
	//take padding number(PKCS#7 Standard)
	int padding_num = block_len - (result.length() % block_len);
	//do padding(PKCS#7 Standard)
	for(int i = 0; i < padding_num; i++)
		result.push_back(static_cast<char>(padding_num));

	return result;
}

size_t mode::real_size(const std::vector<uint64_t>& msg)
{
	if(msg.empty())
		return 0;

	uint8_t pad = msg[msg.size() - 1] & 0xFF;

	if(pad < 1 || pad > 8)
		pad = 0;

	return msg.size() * 8 - pad;
}

inline uint64_t mode::random(void)
{
	uint64_t result;
	
	std::random_device rd;
	//seelct engine for mt19937
    std::mt19937 engine(rd()); 
	//set distribution
    std::uniform_int_distribution<uint64_t> dist(0, 0xFFFFFFFFFFFFFFFF);
	
	result = dist(engine);

	return result;
}

std::vector<uint64_t> mode::to_integer(std::string msg)
{
	//first do padding
	msg = padding(msg);
	
	//save char to integer result
	std::vector<uint64_t> result;
	
	//for using repeatence
	int block_count = msg.length() / block_len;
	
	//resize & initalize vector
	result.resize(block_count, 0);

	//save string each block
	//casting to uint64_t(unless msg casted to 32bit)
	//shift 8 * (8 - j - 1)
	for(int i = 0; i < block_count; i++)
		for(int j = 0; j < block_len; j++)
			result.at(i) |= (static_cast<uint64_t>(static_cast<unsigned char>(msg[block_len * i + j])) << ((sizeof(uint64_t) / sizeof(char)) * (block_len - j - 1)));

	return result;
}

std::string mode::from_integer(std::vector<uint64_t> vec)
{
	std::string result;
	
	size_t size = real_size(vec);
	size_t byte_w = 0;

	result.reserve(size);

	for(auto it = vec.begin(); it != vec.end() && byte_w < size; it++)
	{
		for(size_t j = 0; j < block_len && byte_w < size; j++, byte_w++)
		{
			uint8_t temp = (*it >> (block_len * (block_len - j - 1)) ) & 0xFF;
			result.push_back(static_cast<char>(temp));
		}
	}

	return result;
}

std::vector<uint64_t> mode::ECB(std::string msg, uint64_t& key, uint64_t& key2)
{
	std::vector<uint64_t> imsg;
	//change string to integer vector & padding
	imsg = to_integer(msg);

    size_t total_blocks = imsg.size();
	//exception handling
    if (total_blocks == 0) 
		return imsg;

    std::vector<uint64_t> result(total_blocks);

	//for counting in multi thread env
	//loading menu
	std::atomic<size_t> completed_count(0);
	

	//check hardware's thread count
    unsigned int hw_threads = std::thread::hardware_concurrency();
	    
    int num_threads = (hw_threads >= 4) ? 4 : 2;
	
	//if block is smaller than thread, overhead exist
    if (total_blocks < static_cast<size_t>(num_threads))
        num_threads = static_cast<int>(total_blocks);

	//vector for thread
    std::vector<std::thread> t;
    
	size_t chunk_size = total_blocks / num_threads;
    size_t remainder = total_blocks % num_threads; 

    size_t current_start = 0;

    for (int i = 0; i < num_threads; ++i) 
    {
        size_t current_chunk = chunk_size + (i < remainder ? 1 : 0);
        size_t start = current_start;
        size_t end = start + current_chunk;
	//start threading
	//lambda
        t.emplace_back([this, &imsg, &result, start, end, key, key2, &completed_count]() {
				for (size_t j = start; j < end; ++j)
				{
					//cipher logic
					result[j] = this->cipher(this->decipher(this->cipher(imsg[j], key), key2), key);

					//increase count
					completed_count.fetch_add(1, std::memory_ordered_relaxed);
				}
        });

		//update end index
        current_start = end; 
    }

	//loading logic
	if(m_callback)
	{
		while(completed_count < total_blocks)
		{
			double fraction = static_cast<double>(completed_count.load()) / total_blocks;

			m_callback->update(fraction);
			//10 fps limitation
			std::this_thread::sleep_for(std::chrono::millisecond(100));
		}

		m_callback->updata(1.0);
	}

	//if it allow to join, join thread
    for (auto& it : t)
        if (it.joinable()) 
			it.join();

    return result;
}

std::string mode::ECB(std::vector<uint64_t> msg, uint64_t& key, uint64_t& key2)
{
	std::vector<uint64_t> result;
    size_t total_blocks = msg.size();
	//exception handling
    if (total_blocks == 0) 
		return " ";

	result.resize(total_blocks, 0);

	//for counting in multi thread env
	//loading menu
	std::atomic<size_t> completed_count(0);
	
	//check hardware's thread count
    unsigned int hw_threads = std::thread::hardware_concurrency();
	    
    int num_threads = (hw_threads >= 4) ? 4 : 2;
	
	//if block is smaller than thread, overhead exist
    if (total_blocks < static_cast<size_t>(num_threads))
        num_threads = static_cast<int>(total_blocks);
	//vector for thread
    std::vector<std::thread> t;
    size_t chunk_size = total_blocks / num_threads;
    size_t remainder = total_blocks % num_threads; 

    size_t current_start = 0;
    
	for (int i = 0; i < num_threads; ++i) 
    {
        size_t current_chunk = chunk_size + (i < remainder ? 1 : 0);
        size_t start = current_start;
        size_t end = start + current_chunk;
	//start threading
        t.emplace_back([this, &msg, &result, start, end, key, key2, &completed_count]() {
				for(size_t j = start; j < end; ++j)
				{
					result[j] = this->decipher(this->cipher(this->decipher(msg[j], key), key2), key);
					//increase count
					completed_count.fetch_add(1, std::memory_ordered_relaxed);
				}
        });

		//save end index
        current_start = end; 
    }

	//loading logic
	if(m_callback)
	{
		while(completed_count < total_blocks)
		{
			double fraction = static_cast<double>(completed_count.load()) / total_blocks;

			m_callback->update(fraction);
			//10 fps limitation
			std::this_thread::sleep_for(std::chrono::millisecond(100));
		}

		m_callback->updata(1.0);
	}

	//if it allow to join, join thread
    for (auto& it : t)
        if (it.joinable()) 
			it.join();

	return from_integer(result);
}

std::vector<uint64_t> mode::CBC(std::string msg, uint64_t& key, uint64_t& key2)
{
	std::vector<uint64_t> imsg;
	
	//change string to integer vector
	imsg = to_integer(msg);
	
	//save IV for deciphering
	imsg.insert(imsg.begin(), random());
	
	//take counter of block
	size_t total_blocks = imsg.size();
	
	//exception handling
	if(total_blocks == 0)
		return imsg;

	auto lastUpdateTime = std::chrono::steady_clock::now();

	for(size_t i = 1; i < total_blocks; i++)
	{
		imsg[i] = this->cipher(this->decipher(this->cipher((imsg[i] ^ imsg[i - 1]), key), key2), key);
		//check current time
		auto currentTime = std::chrono::steady_clock::now();
		
		std::chrono::duration<double> elapsed = currentTime - lastUpdateTime;
		//10 fps loading & block count loading
		if(m_callback && (elapsed.count() >= 0.1 || i % (total_blocks / 10 + 1) == 0))
		{
			double fraction = static_cast<double>(i) / (total_blocks - 1);
			m_callback->update(fraction);
			//update time
			lastUpdateTime = currentTime;
		}
	}

	//explicitly print 100%
	m_callback->update(1.0);

	return imsg;
}

std::string mode::CBC(std::vector<uint64_t> msg, uint64_t& key, uint64_t& key2)
{
	std::vector<uint64_t> result;
	size_t total_blocks = msg.size();
	if(total_blocks == 0)
		return " ";
	//save IV for variable
	uint64_t IV = msg[0];
	
	//temp
	uint64_t deciphered;

	//erase IV in vector
	msg.erase(msg.begin());
	//cause erase caculation
	result.resize(total_blocks - 1, 0);
	
	auto lastUpdateTime = std::chrono::steady_clock::now();
	for(size_t i = 0; i < total_blocks - 1; i++)
	{
		//save deciphered value
		//-> CBC use ciphered block
		deciphered = this->decipher(this->cipher(this->decipher(msg[i], key), key2), key);

		if(i)	//decipher block ^ ciphered block
			result[i] = (deciphered ^ msg[i - 1]);
		else	//first xor with IV
			result[i] = deciphered ^ IV;

		//check current time
		auto currentTime = std::chrono::steady_clock::now();
		
		std::chrono::duration<double> elapsed = currentTime - lastUpdateTime;
		//10 fps loading & block count loading
		if(m_callback && (elapsed.count() >= 0.1 || i % (total_blocks / 10 + 1) == 0))
		{
			double fraction = static_cast<double>(i) / (total_blocks - 1);
			m_callback->update(fraction);
			//update time
			lastUpdateTime = currentTime;
		}
	}

	m_callback->update(1.0);

	//change integer vector to string
	return from_integer(result);
}

std::vector<uint64_t> mode::CFB(std::string msg, uint64_t& key, uint64_t& key2)
{
	std::vector<uint64_t> imsg;

	//change string to integer vector
	imsg = to_integer(msg);
	//add random IV value
	imsg.insert(imsg.begin(), random());

	size_t total_blocks = imsg.size();
	//exception handling
	if(total_blocks == 0)
		return imsg;
	
	auto lastUpdateTime = std::chrono::steady_clock::now();

	//xor with ciphered block
	for(size_t i = 1; i < total_blocks; i++)
	{
		imsg[i] = this->cipher(this->decipher(this->cipher(imsg[i - 1], key), key2), key) ^ imsg[i];

		//check current time
		auto currentTime = std::chrono::steady_clock::now();
		
		std::chrono::duration<double> elapsed = currentTime - lastUpdateTime;
		//10 fps loading & block count loading
		if(m_callback && (elapsed.count() >= 0.1 || i % (total_blocks / 10 + 1) == 0))
		{
			double fraction = static_cast<double>(i) / (total_blocks - 1);
			m_callback->update(fraction);
			//update time
			lastUpdateTime = currentTime;
		}
	}

	m_callback->update(1.0);

	return imsg;
}

std::string mode::CFB(std::vector<uint64_t> msg, uint64_t& key, uint64_t& key2)
{
	std::vector<uint64_t> result;

	size_t total_blocks = msg.size();
	
	if(total_blocks == 0)
		return " ";

	result.resize(total_blocks - 1, 0);
	
	auto lastUpdateTime = std::chrono::steady_clock::now();

	//same cipher logic (CFB standard)
	for(size_t i = 1; i < total_blocks; i++)
	{
		result[i - 1] = this->cipher(this->decipher(this->cipher(msg[i - 1], key), key2), key) ^ msg[i];
	
		//check current time
		auto currentTime = std::chrono::steady_clock::now();
		
		std::chrono::duration<double> elapsed = currentTime - lastUpdateTime;
		//10 fps loading & block count loading
		if(m_callback && (elapsed.count() >= 0.1 || i % (total_blocks / 10 + 1) == 0))
		{
			double fraction = static_cast<double>(i) / (total_blocks - 1);
			m_callback->update(fraction);
			//update time
			lastUpdateTime = currentTime;
		}
	}
	return from_integer(result);
}

std::vector<uint64_t> mode::OFB(std::string msg, uint64_t& key, uint64_t& key2)
{
	std::vector<uint64_t> imsg;
	
	//change string to integer vector
	imsg = to_integer(msg);
	//insert IV
	uint64_t IV = random();
	imsg.insert(imsg.begin(), IV);

	size_t total_blocks = imsg.size();
	//exception handling
	if(total_blocks == 0)
		return imsg;

	auto lastUpdateTime = std::chrono::steady_clock::now();

	for(size_t i = 1; i < total_blocks; i++)
	{
		IV = this->cipher(this->decipher(this->cipher(IV, key), key2), key);
		imsg[i] = imsg[i] ^ IV;
		//check current time
		auto currentTime = std::chrono::steady_clock::now();
		
		std::chrono::duration<double> elapsed = currentTime - lastUpdateTime;
		//10 fps loading & block count loading
		if(m_callback && (elapsed.count() >= 0.1 || i % (total_blocks / 10 + 1) == 0))
		{
			double fraction = static_cast<double>(i) / (total_blocks - 1);
			m_callback->update(fraction);
			//update time
			lastUpdateTime = currentTime;
		}
	}
	
	m_callback->update(static_cast<double>(1));

	secure_wipe(&key, sizeof(key));
	secure_wipe(&key2, sizeof(key2));

	return imsg;
}

std::string mode::OFB(std::vector<uint64_t> msg, uint64_t& key, uint64_t& key2)
{
	size_t total_blocks = msg.size();
	if(total_blocks == 0)
		return " ";
		
	auto lastUpdateTime = std::chrono::steady_clock::now();
	
	for(size_t i = 1; i < total_blocks; i++)
	{
		msg[0] = this->cipher(this->decipher(this->cipher(msg[0], key), key2), key);
		msg[i] = msg[i] ^ msg[0];

		//check current time
		auto currentTime = std::chrono::steady_clock::now();
		
		std::chrono::duration<double> elapsed = currentTime - lastUpdateTime;
		//10 fps loading & block count loading
		if(m_callback && (elapsed.count() >= 0.1 || i % (total_blocks / 10 + 1) == 0))
		{
			double fraction = static_cast<double>(i) / (total_blocks - 1);
			m_callback->update(fraction);
			//update time
			lastUpdateTime = currentTime;
		}
	}

	m_callback->update(static_cast<double>(1));
	explicit_bzero(&key, sizeof(uint64_t));
	explicit_bzero(&key2, sizeof(uint64_t));

	//before change to string, erase IV
	msg.erase(msg.begin());

	std::string result = from_integer(msg);

	return result;
}

std::vector<uint64_t> mode::CTR(std::string msg, uint64_t& key, uint64_t& key2)
{
	std::vector<uint64_t> imsg;

	//change string to integer vector & padding
	imsg = to_integer(msg);

    size_t total_blocks = imsg.size();
	//exception handling
    if (total_blocks == 0) 
		return imsg;

    std::vector<uint64_t> result(total_blocks);
	
	//take uint64_t random value
	uint64_t counter = random();

	//for counting in multi thread env
	//loading menu
	std::atomic<size_t> completed_count(0);
	
	//check hardware's thread count
    unsigned int hw_threads = std::thread::hardware_concurrency();
	    
    int num_threads = (hw_threads >= 4) ? 4 : 2;
	
	//if block is smaller than thread, overhead exist
    if (total_blocks < static_cast<size_t>(num_threads))
        num_threads = static_cast<int>(total_blocks);
	//vector for thread
    std::vector<std::thread> t;
    size_t chunk_size = total_blocks / num_threads;
    size_t remainder = total_blocks % num_threads; 

    size_t current_start = 0;

    for (int i = 0; i < num_threads; ++i) 
    {
        size_t current_chunk = chunk_size + (i < remainder ? 1 : 0);
        size_t start = current_start;
        size_t end = start + current_chunk;
	//start threading
        t.emplace_back([this, &imsg, &result, start, end, key, key2, &counter, &completed_count]() {
				for(size_t j = start; j < end; ++j)
				{
					result[j] = imsg[j] ^ this->cipher(this->decipher(this->cipher(counter++, key), key2), key);
					//increase count
					completed_count.fetch_add(1, std::memory_ordered_relaxed);
				}
        });

		//save end index
        current_start = end; 
    }
	//loading logic
	if(m_callback)
	{
		while(completed_count < total_blocks)
		{
			double fraction = static_cast<double>(completed_count.load()) / total_blocks;

			m_callback->update(fraction);
			//10 fps limitation
			std::this_thread::sleep_for(std::chrono::millisecond(100));
		}

		m_callback->updata(1.0);
	}
	//if it allow to join, join thread
    for (auto& it : t)
        if (it.joinable()) 
			it.join();
	
	counter -= result.size();

	result.insert(result.begin(), counter);

    return result;
}



std::string mode::CTR(std::vector<uint64_t> msg, uint64_t& key, uint64_t& key2)
{
	//take counter
	uint64_t counter = msg.front();
	msg.erase(msg.begin());

	std::vector<uint64_t> result;
    size_t total_blocks = msg.size();
	//exception handling
    if (total_blocks == 0) 
		return " ";

	result.resize(total_blocks, 0);

	//for counting in multi thread env
	//loading menu
	std::atomic<size_t> completed_count(0);
	
	//check hardware's thread count
    unsigned int hw_threads = std::thread::hardware_concurrency();
	    
    int num_threads = (hw_threads >= 4) ? 4 : 2;
	
	//if block is smaller than thread, overhead exist
    if (total_blocks < static_cast<size_t>(num_threads))
        num_threads = static_cast<int>(total_blocks);
	//vector for thread
    std::vector<std::thread> t;
    size_t chunk_size = total_blocks / num_threads;
    size_t remainder = total_blocks % num_threads; 

    size_t current_start = 0;
    
	for (int i = 0; i < num_threads; ++i) 
    {
        size_t current_chunk = chunk_size + (i < remainder ? 1 : 0);
        size_t start = current_start;
        size_t end = start + current_chunk;
	//start threading
        t.emplace_back([this, &msg, &result, start, end, key, key2, &counter, &completed_count]() {
				for(size_t j = start; j < end; ++j)
				{
					result[j] = msg[j] ^ this->cipher(this->decipher(this->cipher(counter++, key), key2), key);
					//increase count
					completed_count.fetch_add(1, std::memory_ordered_relaxed);
				}
        });

		//save end index
        current_start = end; 
    }
	//loading logic
	if(m_callback)
	{
		while(completed_count < total_blocks)
		{
			double fraction = static_cast<double>(completed_count.load()) / total_blocks;

			m_callback->update(fraction);
			//10 fps limitation
			std::this_thread::sleep_for(std::chrono::millisecond(100));
		}

		m_callback->updata(1.0);
	}
	//if it allow to join, join thread
    for (auto& it : t)
        if (it.joinable()) 
			it.join();

	return from_integer(result);
}
