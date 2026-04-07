#include "mode.h"
#include "../DES/DES.h"
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

uint64_t mode::random(void)
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
