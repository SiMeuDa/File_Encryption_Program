#ifndef ECB_H
#define ECB_H
#include "cryptologic/mode/mode.h"
#include "cryptologic/crypto.h"

class ECB : public mode{
private:
	bool isFirst;
public:
	ECB(crypto::crypto_logic cl, block key);
	~ECB();

	void encrypt_mode(
		const block, 
		size_t,
		block&, 
		size_t&, 
		bool
	) override;

	void decrypt_mode(
		const block, 
		size_t, 
		block&, 
		size_t&, 
		bool
	) override;
};
#endif
