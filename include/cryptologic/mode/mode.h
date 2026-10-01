#ifndef MODE_H
#define MODE_H
#include "cryptologic/crypto.h"
#include "cryptologic/block.h"
#include "cryptologic/DES/DES.h"
#include "cryptologic/DES/Triple_DES.h"
#include <vector>
#include <string>
#include <string_view>
#include <memory>

class mode{
protected:
	std::unique_ptr<crypto> crypto_ptr;

	size_t block_len;
	//padding: PKCS#7 Standard
	//do padding
	BLOCK padding(const BLOCK&);
	//do unpadding
	BLOCK unpadding(const BLOCK&);

	//return random value
	BLOCK random(void);
public:
	//change msg to BLOCK(vector<byte>)
	BLOCK to_block(std::string_view);
	//change BLOCK(vector<byte>) to msg
	std::string from_block(BLOCK);
	
	mode() { block_len = 0; }
	//set crypto logic
	mode(crypto::crypto_logic cl, BLOCK key);
	virtual ~mode() {}

	virtual BLOCK encrypt_mode(const BLOCK&) = 0;
	virtual BLOCK decrypt_mode(const BLOCK&) = 0;
/*
	//Electric CodeBook mode
	//Cipher Block Chaining mode
	//Cipher FeedBack mode
	//Output FeedBack mode
	//CounTeR
*/
};
#endif
