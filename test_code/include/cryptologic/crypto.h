#pragma once
#include <cstdint>
#include <vector>
#include <string>

class crypto {
private:
	enum cryptologic { DES, AES, HASH };
public:
	virtual ~crypto() {}

	virtual uint64_t cipher(uint64_t) = 0;
	virtual uint64_t decipher(uint64_t) = 0;
public:
	void setParity(uint64_t&);
};
