#include "cryptologic/mode/mode.h"
#include "cryptologic/DES/DES.h"
#include "cryptologic/DES/Triple_DES.h"
#include <cstdint>
#include <cstring>
#include <random>
#include <stdexcept>

mode::mode(crypto::crypto_logic cl, block key)
{
	if(cl == crypto::crypto_logic::DES)
		crypto_ptr = new DES(key);
	else if(cl == crypto::crypto_logic::Triple_DES)
	{
		block div_key[2];
		div_key[0] = new uint8_t[8];

		div_key[1] = new (std::nothrow) uint8_t[8];
		if(div_key[1] == nullptr)
		{
			delete[] div_key[0];

			throw std::bad_alloc();
		}

		for(int i = 0; i < 8; i++)
		{
			(div_key[0])[i] = key[i];
			(div_key[1])[i] = key[i + 8];
		}

		crypto_ptr = new (std::nothrow) Triple_DES(div_key[0], div_key[1]);
		if(crypto_ptr == nullptr)
		{
			delete[] div_key[1];
			delete[] div_key[0];

			throw std::bad_alloc();
		}
	}
	else
		throw std::invalid_argument("Invalid Crypto Logic");
	//set block len
	block_len = crypto_ptr->get_block_size();
}

block mode::padding(block msg, size_t msg_size)
{	
	if(msg == nullptr)
		return msg;
	
	//copy msg
	//take padding number(PKCS#7 Standard)
	uint8_t padding_num = block_len - (msg_size % block_len);
	
	block result = new uint8_t[msg_size + padding_num];

	std::memcpy(result, msg, msg_size);

	//do padding(PKCS#7 Standard)
	for(size_t i = msg_size; i < msg_size + padding_num; i++)
		result[i] = padding_num;

	return result;
}

block mode::unpadding(block msg, size_t msg_size)
{
	if(msg == nullptr || (msg_size % block_len != 0))
		return msg;

	//take padding number(PKCS#7 Standard)
	uint8_t padding_num = msg[msg_size - 1];
	
	//check padding number validation
	if(padding_num == 0 || padding_num > block_len || padding_num > msg_size)
		return msg;

	//check padding byte and msg string
	for(size_t i = msg_size - padding_num; i < msg_size; i++)
		if(msg[i] != padding_num)
			return msg;

	block result = new uint8_t[msg_size - padding_num];

	std::memcpy(result, msg, msg_size - padding_num);

	return result;
}

block mode::random(void)
{
	block result = new uint8_t[block_len];
	
	std::random_device rd;
	
	for(size_t i = 0; i < block_len; i++)
		result[i] = rd() & 0xFF;

	return result;
}
