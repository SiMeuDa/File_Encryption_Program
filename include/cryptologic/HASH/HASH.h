#ifndef HASH_H
#define HASH_H
#include "cryptologic/crypto.h"
#include <string>
#include <string_view>
#if __cplusplus >= 201703L
#include <filesystem>
#endif

class HASH{
private:
	crypto* crypto_ptr;
protected:
	block IV;
	size_t block_bit;
	void merkle(block input, block output);

	void chg_IV(const block IV);
public:
	HASH(const crypto::crypto_logic& cLog, const block IV);
	virtual ~HASH();
	
	virtual void hash(const block input, size_t bit_len, block output, bool isLast) = 0;
	virtual void hash(std::string_view input, block output) = 0;
#if __cplusplus >= 201703L
	virtual void hash(std::filesystem::path p, block output) = 0;
#endif
};

#endif
