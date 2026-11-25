#include "cryptologic/HASH/HES/HES.h"
#include <cstdint>
#include <cstring>
#include <string_view>
#include <stdexcept>
#if __cplusplus >= 201703L
#include <fstream>
#include <filesystem>
#endif

//set crypto logic
HES::HES() : HASH(crypto::crypto_logic::DES, const_cast<block>(init_IV)), total_len(0), byte(nullptr) 
{
	byte = new uint8_t[block_bit];
}
HES::~HES()
{
	if(byte != nullptr)
	{
		delete[] byte;
		byte = nullptr;
	}
}

void HES::hash(block input, size_t bit_len, block output, bool isLast)
{

	total_len += bit_len;

	if(!isLast)
	{ 
		merkle(input, output); 
		return;
	}
	else{
		block size = nullptr;
	        if(size == nullptr)
			size = 	new uint8_t[block_bit]();
		
		for(size_t i = 0; i < bit_len / 8; i++)
			size[i] = input[i];

		if(bit_len % 64)
		{
			//padding with last input
			size[bit_len / 8] |= static_cast<uint8_t>(1ULL << (7 - (bit_len % 8)));
		}
		else
		{
			//last input
			merkle(size, output);
			size[0] = 0x80;

			for(size_t i = 1; i < block_bit; i++)
				size[i] = 0;
		}
		
		merkle(size, output);
		
		std::memcpy(size, &total_len, sizeof(uint64_t));		
		
		merkle(size, output);
		
		chg_IV(const_cast<block>(init_IV));

		delete[] size;

		return;
	}
}

void HES::hash(std::string_view str, block output)
{
	size_t len = str.length();
	bool eof = false;

	for(size_t i = 0; i < len; i++)
	{
		//initalize array
		for(size_t j = 0; j < block_bit; j++)
			byte[j] = 0;

		size_t real_size;
		if(len - i > block_bit)
		{
			real_size = block_bit;
		}else{
			real_size = len - i;
			eof = true;
		}
		
		for(size_t j = 0; j < real_size; j++)
			byte[j] = str[j];

		real_size *= 8;

		hash(byte, real_size, output, eof);
	}
}

#if __cplusplus >= 201703L
void HES::hash(std::filesystem::path p, block output)
{
	bool eof = false;

	if(!(std::filesystem::exists(p) &&
	std::filesystem::is_regular_file(p)))
		throw std::invalid_argument("Invalid file path");

	size_t buffer_size = block_bit * 8;
	block buffer = new uint8_t[buffer_size];

	size_t len = 0;
	
	std::ifstream fin(p, std::ios::binary);

	if(!fin.is_open())
		throw std::runtime_error("Failed to open file");

	do{
		fin.read(reinterpret_cast<char*>(buffer), buffer_size);
		len = fin.gcount();
		eof = fin.eof();

		hash(buffer, buffer_size, output, eof);
	}while(!eof && len != 0);

	fin.close();
	delete[] buffer;
	buffer = nullptr;

}
#endif
