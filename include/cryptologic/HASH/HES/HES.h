#ifndef HES_H
#define HES_H
#include "cryptologic/DES/DES.h"
#include <cstdint>

//merkle damgard construction
class HES{
private:
	DES* des_ptr;
	size_t block_count;
	uint8_t IV[8] = {0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE};
	uint8_t* output;

	uint64_t merkle(uint64_t input);
public:
	HES();
	~HES();
	
	//@param (uint8_t*)input: user input(no limit)
	//@param (size_t)bit_len: bit length(full 64bit = 64)
	//@return (uint8_t*) use uint8_t * 8 for compability to other crypto logic
	uint64_t encrypt(uint64_t input, size_t bit_len, bool isLast);	
};

#endif
