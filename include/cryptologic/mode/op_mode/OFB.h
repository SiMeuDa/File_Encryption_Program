#ifndef OFB_H
#define OFB_H
#include "cryptologic/mode/mode.h"
#include "cryptologic/crypto.h"

class OFB : public mode{
private:
	bool isFirst;

	block IV, init_IV;

	void set_IV(void);
public:
	OFB(crypto::crypto_logic cl, block key);
	~OFB();
	void encrypt_mode(
		const block, size_t,
	       	block&, size_t&,
	       	bool
	) override;
	void decrypt_mode(
		const block, size_t,
	       	block&, size_t&,
		bool
	) override;
};
#endif
