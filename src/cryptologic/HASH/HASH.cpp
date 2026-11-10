#include "cryptologic/HASH/HASH.h"
//crypto logic
#include "cryptologic/crypto.h"
#include "cryptologic/DES/DES.h"
#include "cryptologic/DES/Triple_DES.h"
#include <cstring>
#include <stdexcept>

HASH::HASH(const crypto::crypto_logic& cl, const block input_IV) : crypto_ptr(nullptr), IV(nullptr), block_bit(0)
{
	int a = 10;

	if(cl == crypto::crypto_logic::DES)
		crypto_ptr = new DES(input_IV);
	else if(cl == crypto::crypto_logic::Triple_DES)
		crypto_ptr = new Triple_DES(input_IV, input_IV);
	else
		throw std::invalid_argument("Invalid Crypto Logic");
	
	//set block len
	block_bit = crypto_ptr->get_block_size();

	this->IV = new(std::nothrow) uint8_t[block_bit]();
	if(this->IV == nullptr)
	{
		delete crypto_ptr;
		crypto_ptr = nullptr;

		throw std::bad_alloc();
	}

	std::memcpy(this->IV, input_IV, block_bit);
}

HASH::~HASH()
{
	if(crypto_ptr != nullptr)
	{
		delete crypto_ptr;
		crypto_ptr = nullptr;
	}
	if(IV != nullptr)
	{
		delete[] IV;
		IV = nullptr;
	}
}

void HASH::merkle(const block input, block output)
{
	crypto_ptr->chg_key(input);
	
	crypto_ptr->cipher(IV, output);

	std::memcpy(IV, output, sizeof(uint8_t) * block_bit);
	
	for(size_t i = 0; i < block_bit; i++)
		IV[i] ^= input[i];
}

void HASH::chg_IV(const block IV)
{
	std::memcpy(this->IV, IV, block_bit);
}
