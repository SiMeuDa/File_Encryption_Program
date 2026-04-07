#pragma once
#include "../DES/DES.h"
#include <cstdint>
#include <string>
#include <vector>
#include "mode.h"

class OFB : public mode{
public:
	std::vector<uint64_t> encrypt_mode(std::string, uint64_t&, uint64_t&) override;
	std::string decrypt_mode(std::vector<uint64_t>, uint64_t&, uint64_t&) override;
};
