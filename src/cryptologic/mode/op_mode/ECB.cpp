#include "cryptologic/mode/op_mode/ECB.h"
#include "cryptologic/mode/mode.h"
#include "cryptologic/crypto.h"

ECB::ECB(crypto::crypto_logic cl, block key) : mode(cl, key), isFirst(true){}

ECB::~ECB() {}

void ECB::encrypt_mode(
	const block msg, size_t input_size,
	block& output, size_t& output_size,
       	bool eof
){
	if(output == nullptr)
		return;

	output_size = input_size;

	size_t start = 0;
	
	//last padded block
	if(input_size % block_len != 0)
	{
		size_t pad_index = input_size - (input_size % block_len);
			
		mode::padding(msg + pad_index, input_size - pad_index);

		output_size += input_size % block_len;
	}
	
	//do cipher
	for(; start < output_size; start += block_len)
		crypto_ptr->cipher(msg + start, output + start);
	
}

void ECB::decrypt_mode(
	const block msg, size_t input_size,
	block& output, size_t& output_size, 
	bool eof
){
	//ECB must do padding
	//output should not nullptr
	if((input_size % block_len != 0) ||
	(output == nullptr))
		return;

	size_t start = 0;
	output_size = input_size;

	for(start = 0; start < output_size; start += block_len)
		crypto_ptr->decipher(msg + start, output + start);

	if(eof)
	{
		size_t remain = block_len;

		mode::unpadding(output + output_size - block_len, remain);
		//is there any change occur, change value
		if(remain != block_len)		//update unpadding size
			output_size -= remain;
	}

}
