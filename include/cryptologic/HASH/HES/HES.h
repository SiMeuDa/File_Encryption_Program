#ifndef HES_H
#define HES_H
#include "cryptologic/HASH/HASH.h"
#include "cryptologic/crypto.h"
#include <cstdint>
#include <string_view>
#if __cplusplus >= 201703L
#include <filesystem>
#endif

//merkle damgard construction
class HES : public HASH{
private:
	block byte;
	static constexpr uint8_t init_IV[8] = {0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE};
	uint64_t total_len;

public:
	HES(const crypto::crypto_logic& cLog);
	~HES();
	
	//@param (uint8_t*)input: user input(no limit)
	//@param (size_t)bit_len: bit length(full 64bit = 64)
	//@param (uint8_t*)output: hash function's output
	void hash(block input, size_t bit_len, block output, bool isLast) override;	
	void hash(std::string_view input, block output) override;
#if __cplusplus >= 201703L
	void hash(std::filesystem::path, block output) override;
#endif
};

#endif
