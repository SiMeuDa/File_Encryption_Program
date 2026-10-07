#ifndef MODE_H
#define MODE_H
#include "cryptologic/crypto.h"
#include "cryptologic/DES/DES.h"
#include "cryptologic/DES/Triple_DES.h"
#include <vector>

class mode{
protected:
	crypto* crypto_ptr;

	size_t block_len;
	//padding: PKCS#7 Standard
	//need to free memory
	block padding(block, size_t);
	//need to free memory
	block unpadding(block, size_t);

	//return random value
	//need to free memory
	block random(void);
public:
	mode() { block_len = 0; }
	//set crypto logic
	mode(crypto::crypto_logic cl, block key);
	virtual ~mode() { delete crypto_ptr; }

	virtual void encrypt_mode(const block input, block& output, size_t& size, bool eof) = 0;
	virtual void decrypt_mode(const block, block&, size_t&, bool) = 0;

/*
	//Electric CodeBook mode
	//Cipher Block Chaining mode
	//Cipher FeedBack mode
	//Output FeedBack mode
	//CounTeR
*/
};
#endif
