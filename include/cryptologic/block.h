#ifndef BLOCK_H
#define BLOCK_H
#include <cstddef>
#include <vector>
#include <cstdint>
#include <cmath>
#include <type_traits>
#ifdef LOG
#include <iostream>
#endif
using BLOCK = std::vector<std::byte>;

class block{
public:
	template <typename T>
	static BLOCK to_block(T value)
	{
		static_assert(std::is_trivial_v<T>, "input value's type must a trivial type");

		//built-in type is always trivial type
		size_t size = sizeof(T);
		BLOCK result(size);
		unsigned char bit;
	
		for(size_t i = 0; i < size; i++)
		{
			bit = 0;

			for(size_t j = 0; j < 8; j++)
				if(value & (0x01 << j))
					bit |= (0x01 << 7 - j);

			result[size - 1 - i] = static_cast<std::byte>(bit);

			value = value >> 8;
		}	
	
		return result;
	}

	template <typename T>
	static T to_integer(const BLOCK& value)
	{
		static_assert(std::is_integral_v<T>, "input value's type must a integer type");

		T result{}, temp{};
		unsigned char bit, flip, flag;
		size_t block_size = value.size();

		if(value.size() >= sizeof(T)){
			for(size_t i = 0; i < block_size; i++)
			{
				bit = static_cast<unsigned char>(value[block_size - 1 - i]);
				flip = 0;

				for(size_t j = 0; j < 8; j++)
				{
					flag = static_cast<unsigned char>(bit) & (0x01 << j);
					flag = flag >> j;
					if(flag)
						flip |= flag << (7 - j);
				}
				
				result += flip * static_cast<T>(pow(256, i));
			}
		}else{
			for(size_t i = 0; i < sizeof(T); i++)
			{
				bit = static_cast<unsigned char>(value[block_size - 1 - i]);
				
				result += bit * static_cast<unsigned char>(pow(256, i));
		}}

		return result;
	}
	

	static BLOCK increment(const BLOCK& BLO_value)
	{
		if(BLO_value.empty())
			return BLO_value;

		return block::to_block(block::to_integer<uint64_t>(BLO_value) + 1);
	}

	static BLOCK decrement(const BLOCK& BLO_value)
	{
		if(BLO_value.empty())
			return BLO_value;

		return block::to_block(block::to_integer<uint64_t>(BLO_value) - 1);
	}

#ifdef LOG
	static void print_block(const BLOCK& blo)
	{
		unsigned char bit;

		size_t size = blo.size();

		for(size_t i = 0; i < size; i++)
		{
			bit = static_cast<unsigned char>(blo[size - 1 - i]);

			for(int j = 7; j >=0; j--)
			{
				if(bit & (0x01 << j))
					std::cout << "1";
				else
					std::cout << "0";
			}
			std::cout << " ";
		}
		
	}
#endif
};
#endif
