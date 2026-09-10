#pragma once

#include <cstdint>
#include <vector>
//#include "block.h"
using BLOCK = std::vector<uint8_t>

class crypto {
public:
	virtual ~crypto() = default;

	virtual BLOCK cipher(const BLOCK& block) = 0;
	virtual BLOCK decipher(const BLOCK& block) = 0;
	virtual size_t get_block_size(void) const = 0;
	
	virtual void setParity(BLOCK&) = 0;
};
