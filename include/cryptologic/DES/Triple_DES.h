#ifndef TRIPLE_DES_H
#define TRIPLE_DES_H
#include <cstdint>
#include <vector>
#include <memory>
#include "cryptologic/DES/feistel.h"
#include "cryptologic/crypto.h"
#include "cryptologic/block.h"

class Triple_DES : public crypto {
private:
	crypto* des_ptr[2];
	static constexpr size_t block_size = 8;
	bool setParity(block) override;
public:
	//Make Sub Key vector
	Triple_DES(const block key1, const block key2);
	~Triple_DES();

	BLOCK cipher(const block, block) override;
	BLOCK decipher(const block, block) override;
	bool chkParity(const block) override;
	size_t get_block_size(void) override;
};
#endif
