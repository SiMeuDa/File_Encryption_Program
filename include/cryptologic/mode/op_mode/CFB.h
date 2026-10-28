#ifndef CFB_H
#define CFB_H
#include "cryptologic/mode/mode.h"
#include "cryptologic/crypto.h"

class CFB : public mode{
private:
	bool isFirst;

	block IV, init_IV;

	void set_IV(void);
public:
	CFB(crypto::crypto_logic cl, block key);
	~CFB();
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
