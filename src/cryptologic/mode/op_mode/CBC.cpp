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

	if(isFirst)
	{
		set_IV();

		delete[] output;

		output = new uint8_t[size + block_len];

		std::memcpy(output, IV, block_len * sizeof(uint8_t));

		size += block_len;
	}


	for(size_t i = 0; i < size; i += block_len)
	{
		if(!isFirst)
			for(size_t j = 0; j < block_len; j++)
				IV[j] ^= msg[i + j];
		else
			for(size_t j = 0; j < block_len; j++)
				IV[j] ^= msg[i + j - block_len];

		//do cipher
		crypto_ptr->cipher(IV, output + i);

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
		set_IV();

		std::memcpy(IV, msg, block_len);

		size -= block_len;
	}
	
	for(size_t i = 0; i < size; i += block_len)
	{
		//do cipher
		if(!isFirst)
			crypto_ptr->decipher(msg + i, output + i);
		else
			crypto_ptr->decipher(msg + block_len + i, output + i);

		for(size_t j = 0; j < block_len; j++)
			output[i + j] ^= IV[j];

		if(!isFirst)
			std::memcpy(IV, msg + i, block_len);
		else
			std::memcpy(IV, msg + block_len + i, block_len);
	}

	if(eof)
	{

		delete[] IV;
		isFirst = true;
	}else if(isFirst){
		isFirst = false;
	}
}
