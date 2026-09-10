#include "cryptologic/DES/Triple_DES.h"
#include <cstdint>
#include <vector>
#include <string>
#include <bitset>

Triple_DES::Triple_DES(uint64_t key, uint64_t key2)
{
	if(key2 == 0)
		key2 = key;

	des[0] = new DES(key);
	des[1] = new DES(key2);
}

BLOCK Triple_DES::cipher(BLOCK msg)
{
	msg = des[0]->cipher(msg);
	msg = des[1]->decipher(msg);
	msg = des[0]->cipher(msg);

	return msg;
}

BLOCK Triple_DES::decipher(BLOCK msg)
{
	msg = des[0]->decipher(msg);
	msg = des[1]->cipher(msg);
	msg = des[0]->decipher(msg);

	return msg;
}

bool Triple_DES::chkParity(const BLOCK& key)
{
	for(size_t i = 0; i < key.size(); i++)
	{
		uint8_t group = key[i];

		group ^= group >> 4;
		group ^= group >> 2;
		group ^= group >> 1;

		if((group & 1) == 0)
        		return false;
	}

	return true;
}

size_t Triple_DES::get_block_size(void) { return block_size; }
