#include "cryptologic/mode/ECB.h"
#include "cryptologic/mode/mode.h"
#include "cryptologic/crypto.h"

ECB::ECB(crypto::crypto_logic cl, block key) : mode(cl, key), isFirst(true){}

ECB::~ECB() {}

void ECB::encrypt_mode(const block msg, block& output, size_t& size, bool eof)
{
	block temp;
	size_t start = 0;

	for(start = 0; start + block_len <= size; start += block_len)
		crypto_ptr->cipher(msg + start, output + start);
	
	int check = 10;

	if(eof)
	{
		size_t remain = size - start;

		temp = mode::padding(msg + start, remain);
		
		crypto_ptr->cipher(temp, output + start);

		delete[] temp;
	}
}

void ECB::decrypt_mode(const block msg, block& output, size_t& size, bool eof)
{
	//ECB must do padding
	if(!size % block_len)
		return;

	block temp;
	
	size_t start = 0;
	
	for(start = 0; start + block_len <= size; start += block_len)
		crypto_ptr->decipher(msg + start, output + start);

	if(eof)
	{
		size_t remain = size - start;
		temp = mode::unpadding(msg + start, remain);

		crypto_ptr->decipher(temp, output + start);
	}

}
