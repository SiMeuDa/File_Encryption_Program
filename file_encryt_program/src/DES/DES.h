#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "feistel.h"

class DES : public feistel{
private:
	bool chkParity(uint64_t);
	std::vector<uint64_t> subKey;
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


protected:
	//Make Sub Key vector
	DES() { subKey.resize(16);	}
	uint64_t cipher(uint64_t, uint64_t);
	uint64_t decipher(uint64_t, uint64_t);
	~DES() {}
};
