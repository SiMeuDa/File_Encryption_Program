#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "cryptologic/DES/feistel.h"
#include "cryptologic/crypto.h"

class DES : public feistel, public crypto {
private:
	std::vector<uint64_t> EnsubKey;
	std::vector<uint64_t> DesubKey;

//Left Circular Shift
	uint32_t LCS(uint32_t, size_t);
//not for cipher logic (was for hardware)
//Initial Permutation
	uint64_t IP(uint64_t);
//key scheduling (64bit -> 48bit)
	bool keySchedule(uint64_t);
//not for cipher logic (was for hardware)
//Final Permutation
	uint64_t FP(uint64_t);
public:
	//Make Sub Key vector
	DES() {}
	DES(uint64_t key) : EnsubKey(16), DesubKey(16) { this->keySchedule(key); }
	BLOCK cipher(const BLOCK& block) override;
	BLOCK decipher(const BLOCK& block) override;
	size_t get_block_size(void) override;
	bool chkParity(BLOCK& msg) override;
};
