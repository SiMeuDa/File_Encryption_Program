//crypto API
#include "cryptologic/mode/op_mode/CFB.h"
#include "cryptologic/mode/mode.h"
#include "cryptologic/crypto.h"

#include <cstdint>
#include <cstring>
#include <stdexcept>

CFB::CFB(crypto::crypto_logic cl, block key) : mode(cl, key), isFirst(true), IV(nullptr), init_IV(nullptr){}

CFB::~CFB()
{
	if(IV != nullptr)
	{
		delete[] IV;
		IV = nullptr;
	}

	if(init_IV != nullptr)
	{
		delete[] init_IV;
		init_IV = nullptr;
	}
}

//initialize IV
void CFB::set_IV(void)
{ 
	IV = random(); 
	init_IV = new(std::nothrow) uint8_t[block_len];
	if(init_IV == nullptr)
	{
		delete[] IV;
		IV = nullptr;
	}
}


void CFB::encrypt_mode(
	const block msg, size_t input_size,
       	block& output, size_t& output_size, 
	bool eof
){
	if(msg == nullptr || output == nullptr ||	
		input_size == 0 || block_len == 0)
		return;

	output_size = input_size;

	//set IV
	if(isFirst)
	{
		set_IV();

		std::memcpy(init_IV, IV, block_len);

		output_size += block_len;
		
		//set output memory space
		delete[] output;

		output = new(std::nothrow) uint8_t[output_size];
		if(output == nullptr)
		{
			delete[] IV;
			IV = nullptr;

			delete[] init_IV;
			init_IV = nullptr;

			throw std::bad_alloc();
		}
	}

	//do encryption
	for(size_t i = 0; i < input_size; i += block_len)
	{
		crypto_ptr->cipher(IV, output + i);
		
		size_t real_size = (input_size - i > block_len)? block_len : input_size - i;

		for(size_t j = 0; j < real_size; j++)
			output[i + j] ^= msg[i + j];

		std::memcpy(IV, output + i, real_size);
	}

	//set IV in front of output
	if(isFirst)
	{
		std::memmove(output + block_len, output, input_size);
		std::memcpy(output, init_IV, block_len);

		delete[] init_IV;
		init_IV = nullptr;

		isFirst = false;
	}

	if(eof)
	{
		delete[] IV;
		IV = nullptr;

		isFirst = true;
	}
}

void CFB::decrypt_mode(
	const block msg, size_t input_size,
       	block& output, size_t& output_size, 
	bool eof
){

	if(input_size == 0 || block_len == 0 || 
	(input_size <= block_len && isFirst) ||
	(input_size % block_len))
		return;

	output_size = input_size;

	//set initialize vector
	if(isFirst)
	{
		set_IV();
		
		std::memcpy(IV, msg, block_len);
		
		output_size -= block_len;

		std::memmove(msg, msg + block_len, output_size);

		isFirst = false;
		
		//set output memory space
		delete[] output;

		output = new(std::nothrow) uint8_t[output_size];
		if(output == nullptr)
		{
			delete[] IV;
			IV = nullptr;

			throw std::bad_alloc();
		}
	}
	
	//do decryption
	for(size_t i = 0; i < output_size; i += block_len)
	{
		crypto_ptr->cipher(IV, output + i);
		
		size_t real_size = (input_size - i > block_len)? block_len : input_size - i;

		for(size_t j = 0; j < real_size; j++)
			output[i + j] ^= msg[i + j];

		std::memcpy(IV, msg + i, real_size);
	}

	if(eof)
	{
		delete[] init_IV;
		init_IV = nullptr;
		delete[] IV;
		IV = nullptr;

		isFirst = true;
	}
}
