#ifndef CTR_H
#define CTR_H
#include "cryptologic/mode/mode.h"
#include "cryptologic/crypto.h"

class CTR : public mode{
private:
	bool isFirst;
	block counter;

public:
	CTR(crypto::crypto_logic cl, block key) : mode(cl, key), isFirst(true) {}
	BLOCK encrypt_mode(const BLOCK&, bool) override;
	BLOCK decrypt_mode(const BLOCK&, bool) override;
};
#endif
