#ifndef CRYPTO_H
#define CRYPTO_H
#include <memory>
#include <cstdint>
using block = uint8_t*;

class crypto {
public:
	virtual ~crypto() = default;

	virtual void cipher(const block, block) const = 0;
	virtual void decipher(const block, block) const = 0;
	virtual size_t get_block_size(void) const noexcept = 0;
	//check key's parity bits
	virtual bool chkParity(const block) = 0;
	virtual void setParity(block) = 0;
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

	enum class crypto_logic {
		DES 		= 0,
		Triple_DES 	= 1,
		END		= 2
	};


};
#endif
