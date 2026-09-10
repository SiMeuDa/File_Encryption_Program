#pragma once
#include <cstdint>
#include <vector>
#include <stdexpcept>
#include "cryptologic/DES/feistel.h"
#include "cryptologic/crypto.h"

class DES : public feistel, public crypto {
private:
	constexpr size_t block_size = 8;

	std::vector<uint64_t> EnsubKey;
	std::vector<uint64_t> DesubKey;
	
	template <typename Container>
	void secure_zero(Container& c);
//Left Circular Shift
	uint32_t LCS(uint32_t, size_t);
//not for cipher logic (was for hardware)
//Initial Permutation
	uint64_t IP(uint64_t);
//key scheduling (64bit -> 48bit)
	void keySchedule(uint64_t);
//not for cipher logic (was for hardware)
//Final Permutation
	uint64_t FP(uint64_t);
public:
	//Make Sub Key vector
	DES() {}
	DES(uint64_t key);
	~DES();
	BLOCK cipher(BLOCK block) override;
	BLOCK decipher(BLOCK block) override;
	size_t get_block_size(void) override;
	bool chkParity(const BLOCK& msg) override;
};
