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

	uint8_t temp[8];
       
	des_ptr[0]->cipher(msg, temp);
	des_ptr[1]->decipher(temp, temp);
	des_ptr[0]->cipher(temp, output);
}

void Triple_DES::decipher(const block msg, block output)
{
	if(msg == nullptr ||
	output == nullptr)
		return;
	
	uint8_t temp[8];
       	
	des_ptr[0]->decipher(msg, temp);
	des_ptr[1]->cipher(temp, temp);
	des_ptr[0]->decipher(temp, output);
	
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
