#ifndef DES_H
#define DES_H
#include "cryptologic/crypto.h"
#include "cryptologic/DES/feistel.h"
#include <cstdint>
#include <array>
#include <stdexcept>

class DES : public feistel, public crypto {
private:
	static constexpr size_t block_size = 8;

	std::array<uint64_t, 16> EnsubKey{};
	std::array<uint64_t, 16> DesubKey{};

	bool chkParity(const block) override;
//Left Circular Shift for 28 bit
	uint32_t LCS28(uint32_t, size_t);

//not for cipher logic (was for hardware)
//Initial Permutation
	uint64_t IP(uint64_t);

//key scheduling (64bit -> 48bit)
	void keySchedule(const block);

//not for cipher logic (was for hardware)
//Final Permutation
	uint64_t FP(uint64_t);
public:
	//do s-p table caculate call in first time
	DES() : feistel() {}
	DES(block key);
	~DES();
	void cipher(const block, block) override;
	void decipher(const block, block) override;

	void setParity(block) override;
	size_t get_block_size(void) const noexcept override;
	
	uint64_t load64(const block);
	void store64(uint64_t, block);
};
#endif
