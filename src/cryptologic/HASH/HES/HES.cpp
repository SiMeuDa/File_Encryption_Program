#include "cryptologic/HASH/HES/HES.h"
#include "cryptologic/DES/DES.h"
#include <cstdint>
#include <cstring>
#include <stdexcept>

HES::HES():des_ptr(nullptr), output(nullptr) 
{	
	des_ptr = new DES(IV);
	output = new uint8_t[8];
}

HES::~HES()
{
	if(des_ptr != nullptr)
	{
		delete des_ptr;
		des_ptr = nullptr;
	}

	if(output != nullptr)
	{
		delete[] output;
		output = nullptr;
	}

}

inline uint64_t HES::merkle(uint64_t input)
{
	uint8_t result[8] = {};

	des_ptr->store64(input, output);

	des_ptr->chg_key(output);
	
	des_ptr->cipher(IV, output);

	std::memcpy(IV, output, sizeof(uint8_t) * 8);

	IV[0] ^= result[0];
	IV[1] ^= result[1];
	IV[2] ^= result[2];
	IV[3] ^= result[3];
	IV[4] ^= result[4];
	IV[5] ^= result[5];
	IV[6] ^= result[6];
	IV[7] ^= result[7];

	return des_ptr->load64(IV);
}

uint64_t HES::encrypt(uint64_t input, size_t bit_len, bool isLast)
{

	if(!isLast){ return merkle(input); }
	else{
		if(bit_len % 64)
		{
			//padding with last input
			input |= 1ULL << (64 - (bit_len % 64));
			
			merkle(input);

		}
		else
		{
			//last input
			merkle(input);
			//padding
			merkle(0x8000000000000000);
		}
		//size field
		return merkle(static_cast<uint64_t>(bit_len));
	}
}
