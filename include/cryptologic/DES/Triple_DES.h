#ifndef TRIPLE_DES_H
#define TRIPLE_DES_H
#include <cstdint>
#include "cryptologic/crypto.h"
#include "cryptologic/DES/DES.h"

class Triple_DES : public crypto {
private:
	DES* des_ptr[2];
	static constexpr size_t block_size = 8;
	void setParity(block) override;
public:
	//Make Sub Key vector
	Triple_DES(const block key1, const block key2);
	~Triple_DES();

	void cipher(const block, block) override;
	void decipher(const block, block) override;
	bool chkParity(const block) override;
	size_t get_block_size(void) const noexcept override;
};
#endif
