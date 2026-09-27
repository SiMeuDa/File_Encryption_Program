#pragma once
#include "cryptologic/mode/mode.h"
#include "cryptologic/crypto.h"

class CTR : public mode{
private:
	bool isFirst;
	BLOCK counter;
public:
	CTR(crypto::crypto_logic cl, BLOCK key) : mode(cl, key), isFirst(true) {}
	BLOCK encrypt_mode(const BLOCK&) override;
	BLOCK decrypt_mode(const BLOCK&) override;
};
