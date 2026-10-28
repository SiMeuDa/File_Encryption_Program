#ifndef CTR_H
#define CTR_H
#include "cryptologic/mode/mode.h"
#include "cryptologic/crypto.h"

class CTR : public mode{
private:
	bool isFirst;
	block counter, result_counter, init_counter;
	void set_counter(void);
	void increment(block);
public:
	CTR(crypto::crypto_logic cl, block key);
	~CTR();
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
