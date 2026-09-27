#include "cryptologic/DES/Triple_DES.h"
#include "cryptologic/DES/DES.h"
#include <exception>
#include <cstdint>
#include <vector>

Triple_DES::Triple_DES(BLOCK key, BLOCK key2)
{
	if(key2.empty())
		key2 = key;

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

bool Triple_DES::chkParity(const BLOCK& key)
{
	for(size_t i = 0; i < key.size(); i++)
	{
		std::byte group = key[i];

		group ^= group >> 4;
		group ^= group >> 2;
		group ^= group >> 1;

		if((group & std::byte{0x01}) == std::byte{0x00})
        		return false;
	}

	return true;
}

bool Triple_DES::setParity(BLOCK& key)
{
	for(size_t i = 0; i < key.size(); i++)
        {
                std::byte group = key[i];

                group ^= group >> 4;
                group ^= group >> 2;
                group ^= group >> 1;

                if((group & std::byte{0x01}) == std::byte{0x00})
                                key[i] ^= std::byte{0x01};
        }

        return true;
}

size_t Triple_DES::get_block_size(void) { return block_size; }
