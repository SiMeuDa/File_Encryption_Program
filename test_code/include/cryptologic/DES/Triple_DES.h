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
	Triple_DES(uint64_t key1, uint64_t key2);

	BLOCK cipher(const BLOCK& block) override;
	BLOCK decipher(const BLOCK& block) override;
	bool chkParity(BLOCK& msg) override;
	size_t get_block_size(void) override;
	~Triple_DES() {}
};
