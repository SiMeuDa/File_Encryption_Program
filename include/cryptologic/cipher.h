#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include "mode.h"

class cipher : public mode {
private:
	void setParity(uint64_t&);
public:
	cipher() {};
	~cipher() {};

	std::vector<uint64_t> encryption(std::string, uint64_t, uint64_t = 0);
	std::string decryption(std::vector<uint64_t>, uint64_t, uint64_t = 0);
};
