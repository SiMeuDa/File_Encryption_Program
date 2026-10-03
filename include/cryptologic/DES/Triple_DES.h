#ifndef TRIPLE_DES_H
#define TRIPLE_DES_H
#include <cstdint>
#include "cryptologic/DES/feistel.h"
#include "cryptologic/crypto.h"

class Triple_DES : public crypto {
private:
	crypto* des_ptr[2];
	static constexpr size_t block_size = 8;
	void setParity(block) override;
public:
	//Make Sub Key vector
	Triple_DES(const block key1, const block key2);
	~Triple_DES();

	void cipher(const block, block) const override;
	void decipher(const block, block) const override;
	bool chkParity(const block) override;
	size_t get_block_size(void) const noexcept override;
};
#endif
