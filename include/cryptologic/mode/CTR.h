#ifndef CTR_H
#define CTR_H
#include "cryptologic/mode/mode.h"
#include "cryptologic/crypto.h"
#include "cryptologic/block.h"

class CTR : public mode{
private:
	bool isFirst;
	BLOCK counter;
	BLOCK init_counter;
public:
	CTR(crypto::crypto_logic cl, BLOCK key) : mode(cl, key), isFirst(true) {}
	BLOCK encrypt_mode(const BLOCK&) override;
	BLOCK decrypt_mode(const BLOCK&) override;
};
#endif
