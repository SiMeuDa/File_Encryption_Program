#pragma once
#include "cryptologic/mode/mode.h"
#include "cryptologic/DES/DES.h"
#include <cstdint>
#include <string>
#include <vector>

class CTR : public mode{
public:
	std::vector<uint64_t> encrypt_mode(std::string, uint64_t&, uint64_t&) override;
	std::string decrypt_mode(std::vector<uint64_t>, uint64_t&, uint64_t&) override;
};
