#pragma once

#include <cstdint>
#include <vector>
//#include "block.h"
using BLOCK = std::vector<uint8_t>;

class crypto {
public:
	virtual ~crypto() = default;

	virtual BLOCK cipher(BLOCK block) = 0;
	virtual BLOCK decipher(BLOCK block) = 0;
	virtual size_t get_block_size(void) const = 0;
	//check key's parity bits
	virtual void setParity(const BLOCK&) = 0;

	template <typename Container>
	void secure_erase(Container& c)
	{
		if (c.empty()) 
			return;
    
	       	volatile uint8_t* p = reinterpret_cast<volatile uint8_t*>(c.data());
		size_t byte_len = c.size() * sizeof(typename Container::value_type);
    
	 	while (byte_len--) 
	       	   *p++ = 0;
	}
};
