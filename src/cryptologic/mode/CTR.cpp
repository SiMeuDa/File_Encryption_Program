#include "cryptologic/mode/CTR.h"
#include "cryptologic/mode/mode.h"
#include "cryptologic/crypto.h"
#include "cryptologic/DES/DES.h"
#include "cryptologic/DES/Triple_DES.h"
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <atomic>
#ifdef LOG
#include <iostream>
#endif

CTR::CTR(crypto::crypto_logic cl, block key) : mode(cl, key), isFirst(true){}

CTR::~CTR(){}

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

	std::memcpy(result_counter, counter, block_len * sizeof(int8_t));
}


void CTR::increment(block value)
{
	if(value == nullptr)
		return;
	
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

void CTR::encrypt_mode(const block msg, block& output, size_t& size, bool eof)
{
	if(msg == nullptr || output == nullptr ||	
		size == 0 || block_len == 0)
		return;

	size_t i = 0;

	if(isFirst)
	{
		set_counter();

		delete[] output;

		output = new uint8_t[size + block_len];

		std::memcpy(output, counter, block_len * sizeof(uint8_t));

		size += block_len;

		i = block_len;
	}


	for(; i < size; i += block_len)
	{
		//do cipher
		crypto_ptr->cipher(counter, result_counter);
		
		size_t real_size = (block_len > size - i) ? size - i : block_len;

		//do xor
		if(!isFirst)
			for(size_t j = 0; j < real_size; j++)
				output[i + j] = msg[i + j] ^ result_counter[j];
		else
			for(size_t j = 0; j < real_size; j++)
				output[i + j] = msg[i + j - block_len] ^ result_counter[j];

		//increase counter
		increment(counter);
	}

	if(eof)
	{               
		delete[] counter;
		delete[] result_counter;

		isFirst = true;
	}
	else if(isFirst)
	{
		isFirst = false;
	}
}

void CTR::decrypt_mode(const block msg, block& output, size_t& size, bool eof) 
{ 
	if(size == 0 || block_len == 0 || 
	(size <= block_len && isFirst))
		return;

	
	if(isFirst)
	{
		delete[] output;
		
		set_counter();

		for(size_t i = 0; i < block_len; i++)
			counter[i] = msg[i];
		
		size = size - block_len;

		output = new uint8_t[size];

		std::memcpy(output, msg + block_len, size);

		isFirst = false;

	}
	
	for(size_t i = 0; i < size; i += block_len)
	{
		//do cipher
		crypto_ptr->cipher(counter, result_counter);

		size_t real_size = (block_len > size - i) ? size - i : block_len;

		//do xor
		for(size_t j = 0; j < real_size; j++)
			output[i + j] ^= result_counter[j];

		//increase counter
		increment(counter);
	}

	if(eof)
	{

		delete[] counter;
		delete[] result_counter;
		isFirst = true;
	}
}
