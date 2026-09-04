#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "cryptologic/DES/feistel.h"
#include "cryptologic/crypto.h"

class Triple_DES : private DES, public crypto {
private:
	DES* des_ptr[2];
public:
	//Make Sub Key vector
	Triple_DES(uint64_t, uint64_t);
	uint64_t cipher(uint64_t) override;
	uint64_t decipher(uint64_t) override;
	bool chkParity(uint64_t);
	~Triple_DES() {}
};
