#include "cryptologic/mode/CTR.h"
#include "cryptologic/mode/mode.h"
#include "cryptologic/DES/DES.h"
#include "cryptologic/DES/Triple_DES.h"
#include <cstddef>
#include <cstdint>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#ifdef LOG
#include <iostream>
#endif

BLOCK CTR::encrypt_mode(const BLOCK& msg)
{
	BLOCK result = msg, counter_result, init_counter;	
	
	if(result.empty() || block_len == 0)
		return result;

	if(isFirst)
	{
		counter = random();

		for(auto it = counter.end() - (block_len / 2); it != counter.end(); ++it)
			*it = std::byte{0x00};
		init_counter = counter;
	}

	size_t size = result.size();

	for(size_t i = 0; i < size; i += block_len)
	{
		//do cipher
		counter_result = crypto_ptr->cipher(counter);
		
		size_t real_size = (block_len > size - i) ? size - i : block_len;

		//do xor
		for(size_t j = 0; j < real_size; j++)
			result[i + j] ^= counter_result[j];
		//increase counter
		counter = block::increment(counter);
	}

	if(isFirst)
	{
		result.insert(result.begin(), init_counter.begin(), init_counter.end());
		
		isFirst = false;
	}

	return result;
}

BLOCK CTR::decrypt_mode(const BLOCK& msg) 
{ 
	BLOCK result = msg, counter_result;	

	if(result.empty() || block_len == 0 || 
	(result.size() <= block_len && isFirst))
		return result;
	
	if(isFirst)
	{
#ifdef LOG
		std::clog << "[SYSTEM]: Start to divde counter from vector" << std::endl;
#endif
		counter.resize(block_len);

		for(size_t i = 0; i < block_len; i++)
			counter[i] = result[i];
#ifdef LOG
		std::clog << "[SYSTEM]: Start to erase counter in vector" << std::endl;
#endif
		result.erase(result.begin(), result.begin() + block_len);

		isFirst = false;
#ifdef LOG
		std::clog << "[SYSTEM]: Success to erase counter" << std::endl;
		std::clog << "[SYSTEM]: Success to divide counter" << std::endl;
#endif
	}
	
	size_t size = result.size();

#ifdef LOG
	std::clog << "[SYSTEM]: Start to loop" << std::endl;
#endif
	for(size_t i = 0; i < size; i += block_len)
	{
		//do cipher
		counter_result = crypto_ptr->cipher(counter);

		size_t real_size = (block_len > size - i) ? size - i : block_len;
		//do xor
		for(size_t j = 0; j < real_size; j++)
			result[i + j] ^= counter_result[j];

		//increase counter
		counter = block::increment(counter);
	}
#ifdef LOG
	std::clog << "[SYSTEM]: Success to loop" << std::endl;
#endif
	return result;
}
