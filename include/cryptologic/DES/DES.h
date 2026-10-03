#ifndef DES_H
#define DES_H
#include <cstdint>
#include <vector>
#include <array>
#include <stdexcept>
#include "cryptologic/DES/feistel.h"
#include "cryptologic/crypto.h"

class DES : public feistel, public crypto {
private:
	static constexpr size_t block_size = 8;

	std::array<uint64_t, 16> EnsubKey{};
	std::array<uint64_t, 16> DesubKey{};
	
	void load64(const block, uint64_t&);
	void store64(uint64_t, block);

	bool chkParity(const block) override;
//Left Circular Shift for 28 bit
	void LCS28(uint32_t&, size_t);
//not for cipher logic (was for hardware)
//Initial Permutation
	void IP(uint64_t&);
//key scheduling (64bit -> 48bit)
	void keySchedule(const block);
//not for cipher logic (was for hardware)
//Final Permutation
	void FP(uint64_t&);
public:
	//Make Sub Key vector
	DES() {}
	DES(uint64_t key);
	~DES();
	void cipher(const block, block) override;
	void decipher(const block, block) override;

	bool setParity(block) override;
	size_t get_block_size(void) override;
};
#endif
