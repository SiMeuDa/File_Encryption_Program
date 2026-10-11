//crypto API
#include "cryptologic/mode/op_mode/CBC.h"
#include "cryptologic/mode/mode.h"
#include "cryptologic/crypto.h"

#include <cstdint>
#include <cstring>
#include <stdexcept>
//multi-threading
#include <thread>
#include <chrono>
#include <atomic>

CBC::CBC(crypto::crypto_logic cl, block key) : mode(cl, key), isFirst(true){}

CBC::~CBC(){}

//initialize IV
void CBC::set_IV(void){ IV = random(); }


void CBC::encrypt_mode(const block msg, block& output, size_t& size, bool eof)
{
	if(msg == nullptr || output == nullptr ||	
		size == 0 || block_len == 0)
		return;

	size_t i = 0;

	if(isFirst)
	{
		set_IV();

		delete[] output;

		output = new uint8_t[size + block_len];

		std::memcpy(output, IV, block_len * sizeof(uint8_t));

		size += block_len;

		i = block_len;
	}


	for(; i < size; i += block_len)
	{
		for(size_t j = 0; j < block_len; j++)
			IV[j] ^= msg[i + j];

		//do cipher
		crypto_ptr->cipher(output + i, IV);

		std::memcpy(IV, output + i, block_len);
	}		

	if(eof)
	{
		delete[] IV;
		
		isFirst = true;
	}
	else if(isFirst)
	{
		isFirst = false;
	}
}

void CBC::decrypt_mode(const block msg, block& output, size_t& size, bool eof) 
{ 
	if(size == 0 || block_len == 0 || 
	(size <= block_len && isFirst))
		return;

	
	if(isFirst)
	{
		delete[] output;
		
		set_IV();

		for(size_t i = 0; i < block_len; i++)
			IV[i] = msg[i];
		
		size = size - block_len;

		output = new uint8_t[size];

		std::memcpy(output, msg + block_len, size);

		isFirst = false;

	}
	
	for(size_t i = 0; i < size; i += block_len)
	{
		
		//do cipher
		crypto_ptr->decipher(msg + i, output + i);

		for(size_t j = i; j < size; j++)
			output[j] ^= IV[j];

		std::memcpy(IV, msg + i, block_len);
	}

	if(eof)
	{

		delete[] IV;
		isFirst = true;
	}
}
