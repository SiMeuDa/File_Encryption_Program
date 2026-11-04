#ifndef HASH_H
#define HASH_H
#include "cryptologic/HASH/HES/HES.h"
#include <filesystem>
#include <fstream>
#include <string>

template <typename hs>
class hash{
private:
	hs hash_cons;
	std::ifstream fin;


public:
	hash() = default;
	explicit hash(const hs& h) : hash_cons(h){}
	~hash() {}
	
	uint64_t do_hash(std::filesystem::path p)
	{
		bool eof = false;
		uint64_t data = 0, result = 0;
		size_t bit_len = 0;
		char str[8];
		if(std::filesystem::exists(p) & std::filesystem::is_regular_file(p) != true)
			return 0;
		
		fin.open(p, std::ios::binary);
		if(fin.is_open() != true)
			return 0;

		while(!eof)
		{
			data = 0;
			fin.read(str, 8);
			eof = fin.eof();
			bit_len = static_cast<size_t>(fin.gcount());

			for(size_t j = 0; j < bit_len; j++)
				data |= static_cast<uint64_t>(str[j]) << 8 * (7 - j);
				
			bit_len *= 8;
			
			result = hash_cons.encrypt(data, bit_len, eof);
		}
		
		return result;
	}

	uint64_t do_hash(std::string str)
	{
		uint64_t data = 0, result;

		size_t len = str.length();
		size_t bit_len = 0;
		bool isLast = false;

		for(size_t i = 0; i < len; i+= 8)
		{
			data = 0;
			bit_len = (len > i + 8) ? 8 : len - i;

			for(size_t j = 0; j < bit_len; j++)
				data |= static_cast<uint64_t>(str[i + j]) << 8 * (7 - j);

			bit_len *= 8;

			result = hash_cons.encrypt(data, bit_len, isLast);
				
		}

		return result;
	}
};

#endif
