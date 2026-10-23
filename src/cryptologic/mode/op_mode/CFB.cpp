//crypto API
#include "cryptologic/mode/op_mode/CFB.h"
#include "cryptologic/mode/mode.h"
#include "cryptologic/crypto.h"

#include <cstdint>
#include <cstring>
#include <stdexcept>

CFB::CFB(crypto::crypto_logic cl, block key) : mode(cl, key), isFirst(true){}

CFB::~CFB(){}

//initialize IV
void CFB::set_IV(void){ IV = random(); }


void CFB::encrypt_mode(const block msg, block& output, size_t& size, bool eof)
{
	if(msg == nullptr || output == nullptr ||	
		size == 0 || block_len == 0)
		return;

	if(isFirst)
	{
		set_IV();

		delete[] output;
		
		size += block_len;

		output = new uint8_t[size + block_len];

		std::memcpy(output, IV, block_len);
	}

	for(size_t i = 0; i < size; i++)
	{	
		crypto_ptr->cipher(IV, IV);
	
		for(size_t j = 0; j < block_len; j++)
			IV[j] ^= msg[i + j];
		
		if(!isFirst)
			std::memcpy(output + i, IV, block_len);
		else
			std::memcpy(output + i + block_len, IV, block_len);
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

void CFB::decrypt_mode(const block msg, block& output, size_t& size, bool eof) 
{ 
	if(size == 0 || block_len == 0 || 
	(size <= block_len && isFirst))
		return;

	
	if(isFirst)
	{
		set_IV();
		
		delete[] output;

		std::memcpy(IV, msg, block_len);

		size -= block_len;

		output = new uint8_t[size];
	}
	
	for(size_t i = 0; i < size; i += block_len)
	{
		crypto_ptr->decipher(IV, IV);
		
		if(!isFirst)
			for(size_t j = 0; j < block_len; j++)
				IV[j] ^= msg[i + j];
		else
			for(size_t j = 0; j < block_len; j++)
				IV[j] ^= msg[i + block_len + j];
		
		std::memcpy(output + i, IV, block_len);

		if(!isFirst)
			std::memcpy(IV, msg + i, block_len);
		else
			std::memcpy(IV, msg + block_len + i, block_len);
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
