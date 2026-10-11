#ifndef CBC_H
#define CBC_H
#include "cryptologic/mode/mode.h"
#include "cryptologic/crypto.h"

class CBC : public mode{
private:
	bool isFirst;

	block IV;

	void set_IV(void);
public:
	CBC(crypto::crypto_logic cl, block key);
	~CBC();
	void encrypt_mode(const block, block&, size_t&, bool) override;
	void decrypt_mode(const block, block&, size_t&, bool) override;
};
#endif
