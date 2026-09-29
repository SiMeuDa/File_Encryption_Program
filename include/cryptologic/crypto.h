#pragma once
#include <cstddef>
#include <cstdint>
#include <vector>
#include <array>
//#include "block.h"
using BLOCK = std::vector<std::byte>;

class crypto {
public:
	virtual ~crypto() = default;

	virtual BLOCK cipher(BLOCK block) = 0;
	virtual BLOCK decipher(BLOCK block) = 0;
	virtual size_t get_block_size(void) = 0;
	//check key's parity bits
	virtual bool chkParity(const BLOCK&) = 0;
	virtual bool setParity(BLOCK&) = 0;
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

	template <typename T>
	static BLOCK to_block(T value)
	{
		BLOCK result;
	}

	enum class crypto_logic {
		DES 		= 0,
		Triple_DES 	= 1,
		END		= 2
	};


};
