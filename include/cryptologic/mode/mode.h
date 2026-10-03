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
	//do padding
	void padding(block);
	//do unpadding
	void unpadding(block);

	//return random value
	block random(void);
public:
	mode() { block_len = 0; }
	//set crypto logic
	mode(crypto::crypto_logic cl, block key);
	virtual ~mode() {}

	virtual BLOCK encrypt_mode(const block, bool eof) = 0;
	virtual BLOCK decrypt_mode(const block, bool eof) = 0;
/*
	//Electric CodeBook mode
	//Cipher Block Chaining mode
	//Cipher FeedBack mode
	//Output FeedBack mode
	//CounTeR
*/
};
#endif
