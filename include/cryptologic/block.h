#include <cstddef>
#include <vector>
using BLOCK = std::vector<std:::byte>;

class block{
public:
	template <typename T>
	static BLOCK to_block(T value)
	{
		BLOCK result;
	}

	template <typename T>
	static BLOCK to_integer(const BLOCK& value)
	{
	}
	
	friend BLOCK operator++(BLOCK& BLO_value, int int_value)
	{
		BLOCK data = BLO_value;
		if(data.empty())
			return data;

		size_t block_size = data.size();
		size_t type_size = sizeof(std::byte) * 8;

		for(size_t i = 0; i < block_size; i++)
		{
			for(size_t j = 0; j < type_size; j++)
			{
				if(data[block_size - 1 - i] & (std::byte{0x01} << j)
				!= std::byte{0x00})
				{
					data[block_size - 1 - i] ^= (std::byte{0x01} << j);
					return data;
				}else{
					data[block_size - 1 - i] ^= (std::byte{0x01} << j);
				}
		}}

		return data;
	}

	friend BLOCK& operator++(BLOCK& BLO_value)
	{
		if(BLO_value.empty())
			return BLO_value;

		size_t block_size = BLO_value.size();
		size_t type_size = sizeof(std::byte) * 8;

		for(size_t i = 0; i < block_size; i++)
		{
			for(size_t j = 0; j < type_size; j++)
			{
				if(BLO_value[block_size - 1 - i] & (std::byte{0x01} << j)
				!= std::byte{0x00})
				{
					BLO_value[block_size - 1 - i] ^= (std::byte{0x01} << j);
					return BLO_value;
				}else{
					BLO_value[block_size - 1 - j] ^= (std::byte{0x01} << j);
				}
		}}

		return BLO_value;
	}
}	
