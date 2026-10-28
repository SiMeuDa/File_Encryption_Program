//crypto API
#include "cryptologic/mode/op_mode/CTR.h"
#include "cryptologic/mode/mode.h"
#include "cryptologic/crypto.h"

#include <cstdint>
#include <cstring>
#include <stdexcept>
//multi-threading
#include <thread>
#include <chrono>
#include <atomic>

CTR::CTR(crypto::crypto_logic cl, block key) : 
mode(cl, key), isFirst(true), counter(nullptr), result_counter(nullptr), init_counter(nullptr) {}

CTR::~CTR()
{
	if(counter != nullptr)
	{
		delete[] counter;
		counter = nullptr;
	}
	if(result_counter != nullptr)
	{
		delete[] result_counter;
		result_counter = nullptr;
	}

	if(init_counter != nullptr)
	{
		delete[] init_counter;
		init_counter = nullptr;
	}
}

void CTR::set_counter(void)
{
	//initialize counter
	counter = random();
	result_counter = new(std::nothrow) uint8_t[block_len];
	if(result_counter == nullptr)
	{
		delete[] counter;
		throw std::bad_alloc();
	}

	for(size_t i = block_len - (block_len / 2); i < block_len; i++)
		counter[i] = 0x00;
}


void CTR::increment(block value)
{
	if(value == nullptr)
		return;

	if(value[block_len - 1] != 0xFF)
	{
		++value[block_len - 1];
		return;
	}
	
	for(size_t i = 0; i < block_len; i++)
	{
		if(value[block_len - 1 - i] != 0xFF)
		{
			++value[block_len - 1 - i];
			break;
		}
		else
			value[block_len - 1 - i] = 0;
	}
}

void CTR::encrypt_mode(
	const block msg, size_t input_size,
       	block& output, size_t& output_size,
       	bool eof
){
	if((input_size == 0) && eof)
	{
		isFirst = true;
		return;
	}
	if(msg == nullptr || output == nullptr ||	
		input_size == 0 || block_len == 0)
		return;

	output_size = input_size;

	if(isFirst)
	{
		//set counter
		set_counter();
		if(init_counter == nullptr)
		{
			init_counter = new(std::nothrow) uint8_t[block_len];
			if(init_counter == nullptr)
			{
				delete[] counter;
				counter = nullptr;

				delete[] result_counter;
				result_counter = nullptr;
			}
		}
		std::memcpy(init_counter, counter, block_len);

		//prevent double free
		if(output != nullptr)
		{
			delete[] output;
			output = nullptr;
		}

		output_size += block_len;

		output = new(std::nothrow) uint8_t[output_size];
		if(output == nullptr)
		{
			delete[] counter;
			counter = nullptr;

			delete[] result_counter;
			result_counter = nullptr;

			delete[] init_counter;
			init_counter = nullptr;

			throw std::bad_alloc();
		}
	}


	for(size_t i = 0; i < input_size; i += block_len)
	{
		//do cipher
		crypto_ptr->cipher(counter, result_counter);
		
		//check real size (CTR don't padding)
		size_t real_size = (block_len > input_size - i) ? input_size - i : block_len;

		//do xor
		for(size_t j = 0; j < real_size; j++)
			output[i + j] = msg[i + j] ^ result_counter[j];

		//increase counter
		increment(counter);
	}

	if(isFirst)
	{
		std::memmove(output + block_len, output, input_size);
		std::memcpy(output, init_counter, block_len);

		delete[] init_counter;
		init_counter = nullptr;

		isFirst = false;
	}

	if(eof)
	{               
		delete[] counter;
		counter = nullptr;

		delete[] result_counter;
		result_counter = nullptr;

		isFirst = true;
	}
}

void CTR::decrypt_mode(
	const block msg, size_t input_size,
       	block& output, size_t& output_size, 
	bool eof
){ 
	if((input_size == 0) && eof)
	{
		isFirst = true;
		return;
	}

	if(input_size == 0 || block_len == 0 || 
	(input_size <= block_len && isFirst))
		return;

	output_size = input_size;
	
	if(isFirst)
	{
		delete[] output;
		output = nullptr;

		set_counter();
	
		std::memcpy(counter, msg, block_len);
		
		output_size -= block_len;

		output = new(std::nothrow) uint8_t[output_size];
		if(output == nullptr)
		{
			delete[] counter;
			counter = nullptr;

			delete[] result_counter;
			result_counter = nullptr;
		}
		
		isFirst = false;

		std::memmove(msg, msg + block_len, output_size);
	}

	for(size_t i = 0; i < output_size; i += block_len)
	{
		//do cipher
		crypto_ptr->cipher(counter, result_counter);

		size_t real_size = (block_len > output_size - i) ? output_size - i : block_len;

		//do xor
		for(size_t j = 0; j < real_size; j++)
			output[i + j] = msg[i + j] ^ result_counter[j];

		//increase counter
		increment(counter);
	}

	if(eof)
	{
		delete[] counter;
		counter = nullptr;
		
		delete[] result_counter;
		result_counter = nullptr;

		isFirst = true;
	}
}
