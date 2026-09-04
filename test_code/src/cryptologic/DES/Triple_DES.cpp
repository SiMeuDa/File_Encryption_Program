#include "cryptologic/DES/Triple_DES.h"
#include <cstdint>
#include <vector>
#include <string>
#include <bitset>

Triple_DES::Triple_DES(uint64_t key, uint64_t key2 = 0)
{
	if(key2 == 0)
		key2 = key;

	des[0] = new DES(key);
	des[1] = new DES(key2);
}

uint64_t Triple_DES::cipher(uint64_t msg)
{

	des[0]->cipher(msg);
	des[1]->decipher(msg);
	des[0]->cipher(msg);

	return msg;
}

uint64_t Triple_DES::decipher(uint64_t msg)
{
	des[0]->decipher(msg);
	des[1]->cipher(msg);
	des[0]->decipher(msg);

	return msg;
}
