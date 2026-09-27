#pragma once
#include "cryptologic/mode/mode.h"
#include "cryptologic/crypto.h"

class CFB : public mode{
public:
	BLOCK encrypt_mode(const BLOCK&) override;
	BLOCK decrypt_mode(const BLOCK&) override;
};
