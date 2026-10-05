#include "cryptologic/DES/Triple_DES.h"
#include "cryptologic/DES/DES.h"
#include <cstdint>
#include <stdexcept>

Triple_DES::Triple_DES(const block key, const block key2)
{
	if(key == nullptr ||
	key2 == nullptr)
		throw std::bad_alloc();

	des_ptr[0] = new DES(key);

	des_ptr[1] = new(std::nothrow) DES(key2);
	if(des_ptr[1] == nullptr)
	{
		delete des_ptr[0];

		throw std::bad_alloc();
	}
}

Triple_DES::~Triple_DES()
{
	delete des_ptr[1];
	delete des_ptr[0];
}

void Triple_DES::cipher(const block msg, block output)
{
	if(msg == nullptr ||
	output == nullptr)
		return;

	block temp[2];
       	
	temp[0]	= new uint8_t[8];
	temp[1] = new(std::nothrow) uint8_t[8];
	if(temp[1] == nullptr)
	{
		delete[] temp[0];
		throw std::bad_alloc();
	}

	des_ptr[0]->cipher(msg, temp[0]);
	des_ptr[1]->decipher(temp[0], temp[1]);
	des_ptr[0]->cipher(temp[1], output);

	delete[] temp[1];
	delete[] temp[0];
}

void Triple_DES::decipher(const block msg, block output)
{
	if(msg == nullptr ||
	output == nullptr)
		return;

	block temp[2];
       	
	temp[0]	= new uint8_t[8];
	temp[1] = new(std::nothrow) uint8_t[8];
	if(temp == nullptr)
	{
		delete temp[0];
		throw std::bad_alloc();
	}

	des_ptr[0]->decipher(msg, temp[0]);
	des_ptr[1]->cipher(temp[0], temp[1]);
	des_ptr[0]->decipher(temp[1], output);
	
	delete[] temp[1];
	delete[] temp[0];
}

bool Triple_DES::chkParity(const block key)
{
	for(size_t i = 0; i < 8; i++)
	{
		uint8_t group = key[i];

		group ^= group >> 4;
		group ^= group >> 2;
		group ^= group >> 1;

		if((group & 0x01) == 0x00)
        		return false;
	}

	return true;
}

void Triple_DES::setParity(block key)
{
	for(size_t i = 0; i < 8; i++)
        {
                uint8_t group = key[i];

                group ^= group >> 4;
                group ^= group >> 2;
                group ^= group >> 1;

                if((group & 0x01) == 0x00)
                                key[i] ^= 0x01;
        }
}

size_t Triple_DES::get_block_size(void) const noexcept { return block_size; }
