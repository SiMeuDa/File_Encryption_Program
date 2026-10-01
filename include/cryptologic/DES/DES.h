#ifndef DES_H
#define DES_H
#include <cstdint>
#include <vector>
#include <stdexcept>
#include "cryptologic/DES/feistel.h"
#include "cryptologic/crypto.h"
#include "cryptologic/block.h"

class DES : public feistel, public crypto {
private:
	static constexpr size_t block_size = 8;

	std::vector<uint64_t> EnsubKey;
	std::vector<uint64_t> DesubKey;
	
	bool chkParity(const BLOCK&) override;
//Left Circular Shift for 28 bit
	uint32_t LCS28(uint32_t, size_t);
//not for cipher logic (was for hardware)
//Initial Permutation
	BLOCK IP(BLOCK);
//key scheduling (64bit -> 48bit)
	void keySchedule(BLOCK);
//not for cipher logic (was for hardware)
//Final Permutation
	BLOCK FP(BLOCK);
public:
	//Make Sub Key vector
	DES() {}
	DES(BLOCK key);
	~DES();
	BLOCK cipher(BLOCK block) override;
	BLOCK decipher(BLOCK block) override;

	bool setParity(BLOCK&) override;
	size_t get_block_size(void) override;
};
#endif
