#include "cipher.h"
#include <bitset>
#include <cstdint>
#include <vector>
#include <string>
#include "mode.h"

void cipher::setParity(uint64_t& key)
{
	for (int i = 0; i < 8; i++)
    {
    	uint8_t group = (key >> (i * 8)) & 0xFF;

 	    int ones = std::bitset<8>(group).count();

        uint8_t parity = (ones % 2 == 0) ? 1 : 0;

        key &= ~(1ULL << (i * 8));
        key |=  ((uint64_t)parity << (i * 8));
    }
}

std::vector<uint64_t> cipher::encryption(std::string msg, uint64_t key, uint64_t key2)
{
	setParity(key);
	setParity(key2);

	return mode::OFB(msg, key, key2);
}

std::string cipher::decryption(std::vector<uint64_t> msg, uint64_t key, uint64_t key2)
{
	setParity(key);
	setParity(key2);

	return mode::OFB(msg, key, key2);
}
