#include "cryptologic/mode/mode.h"
#include "cryptologic/DES/DES.h"
#include "cryptologic/DES/Triple_DES.h"
#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>
#include <random>
#include <string_view>
#include <stdexcept>

mode::mode(crypto::crypto_logic cl, BLOCK key)
{
	if(cl == crypto::crypto_logic::DES)
		crypto_ptr = std::make_unique<DES>(key);
	else if(cl == crypto::crypto_logic::Triple_DES)
	{
		BLOCK key2 = key;
		key2.erase(key2.begin(), key2.begin() + 8);

		crypto_ptr = std::make_unique<Triple_DES>(key, key2);
	}
	else
		throw std::invalid_argument("Invalid Crypto Logic");
	//set block len
	block_len = crypto_ptr->get_block_size();
}

BLOCK mode::padding(const BLOCK& msg)
{	
	if(msg.empty())
		return msg;

	//copy msg
	BLOCK result = msg;
	//take padding number(PKCS#7 Standard)
	size_t padding_num = block_len - (result.size() % block_len);
	
	//do padding(PKCS#7 Standard)
	result.insert(result.end(), padding_num, static_cast<std::byte>(padding_num));

	return result;
}

BLOCK mode::unpadding(const BLOCK& msg)
{
	if(msg.empty() || (msg.size() % block_len != 0))
		return msg;

	//take padding number(PKCS#7 Standard)
	size_t padding_num = static_cast<size_t>(msg.back());
	
	//check padding number validation
	if(padding_num == 0 || padding_num > block_len || padding_num > msg.size())
		return msg;

	//check padding byte and msg string
	for(auto it = msg.end() - padding_num; it != msg.end(); ++it)
		if(static_cast<size_t>(*it) != padding_num)
			return msg;

	return BLOCK(msg.begin(), msg.end() - padding_num);
}

BLOCK mode::random(void)
{
	BLOCK result(block_len);
	
	std::random_device rd;
	
	for(size_t i = 0; i < block_len; i++)
		result[i] = static_cast<std::byte>(rd() & 0xFF);

	return result;
}


BLOCK mode::to_block(std::string_view msg)
{
	//for using repeatence
	int block_count = msg.length();

	//Definition and Initialization of Result
	BLOCK result(block_count);
	
	//save string each block
	//casting to byte
	//shift 8 * (8 - j - 1)
	for(int i = 0; i < block_count; i++)
		result[i] |= static_cast<std::byte>(static_cast<unsigned char>(msg[i]));

	return result;
}

std::string mode::from_block(BLOCK blo)
{
	//char = 1byte
	std::string result;
	//byte = 1byte
	size_t size = blo.size();

	//string length == vector size
	result.resize(size);

	for(size_t i = 0; i < size; i++)
		result[i] = static_cast<char>(blo[i]);

	return result;
}
