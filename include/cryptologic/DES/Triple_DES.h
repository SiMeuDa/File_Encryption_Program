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
public:
	//Make Sub Key vector
	Triple_DES(BLOCK key1, BLOCK key2);
	~Triple_DES();

	BLOCK cipher(BLOCK) override;
	BLOCK decipher(BLOCK) override;
	bool chkParity(const BLOCK& msg) override;
	bool setParity(BLOCK&) override;
	size_t get_block_size(void) override;
};
#endif
