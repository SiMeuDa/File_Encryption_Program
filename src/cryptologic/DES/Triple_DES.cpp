#include "cryptologic/DES/Triple_DES.h"
#include "cryptologic/DES/DES.h"
#include <cstdint>

Triple_DES::Triple_DES(const block key, const block key2)
{
	if(key == nullptr ||
	key2 == nullptr)
		throw std::bad_alloc();

	des_ptr[0] = new DES(key);
	if(des_ptr[0] == nullptr)
		throw std::bad_alloc();

	des_ptr[1] = new DES(key2);
	if(des_ptr == nullptr)
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

BLOCK Triple_DES::cipher(BLOCK msg)
{
	msg = des_ptr[0]->cipher(msg);
	msg = des_ptr[1]->decipher(msg);
	msg = des_ptr[0]->cipher(msg);

	return msg;
}

BLOCK Triple_DES::decipher(BLOCK msg)
{
	msg = des_ptr[0]->decipher(msg);
	msg = des_ptr[1]->cipher(msg);
	msg = des_ptr[0]->decipher(msg);

	return msg;
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

size_t Triple_DES::get_block_size(void) { return block_size; }
