#include "cryptologic/DES/DES.h"
#include "cryptologic/crypto.h"
#include <cstdint>
#include <stdexcept>

DES::DES(block key)
{
	setParity(key);

	if(!chkParity(key))
		throw std::invalid_argument("Invalid Key Value");

	this->keySchedule(key);
}

DES::~DES() { crypto::secure_erase(EnsubKey); crypto::secure_erase(DesubKey); }

uint64_t DES::load64(block input)
{
	uint64_t result = 0;
	for(int i = 0; i < 8; i++)
		result |= static_cast<uint64_t>(input[i]) << (8 * i);

	return result;
}

void DES::store64(uint64_t input, block output)
{
	for(int i = 0; i < 8; i++)
		output[i] = (input >> (8 * i)) & 0xFF;
}

uint32_t DES::LCS28(uint32_t value, size_t count)
{
//LCS| Left Circular Shift
//MSB| Most Significant Bit(value[0]'s most left bit)

        uint32_t msb;

        for(size_t i = 0; i < count; i++)
        {
                msb = (value >> 27) & 0x00000001;

		value = value << 1;

		value |= msb;
	
		value &= 0x0FFFFFFF;

        }
        return value;
}


uint64_t DES::IP(uint64_t msg)
{
        //Standard IP Table
        static int table[64] = {
                58, 50, 42, 34, 26, 18, 10, 2,
                60, 52, 44, 36, 28, 20, 12, 4,
                62, 54, 46, 38, 30, 22, 14, 6,
                64, 56, 48, 40, 32, 24, 16, 8,
                57, 49, 41, 33, 25, 17,  9, 1,
                59, 51, 43, 35, 27, 19, 11, 3,
                61, 53, 45, 37, 29, 21, 13, 5,
                63, 55, 47, 39, 31, 23, 15, 7
                };
        
	uint64_t result = 0;
        
	for(int i = 0; i < 64; i++)
		result |= ((msg >> (64 - table[i])) & 1ULL) << (63 - i);

        return result;
}

void DES::keySchedule(block key)
{
	//Standard Table
	static int pc1_Ctable[28] = {
		57, 49, 41, 33, 25, 17,  9,
		 1,	58, 50, 42, 34,	26, 18,
		10,  2,	59, 51, 43, 35, 27,
		19, 11,  3,	60, 52, 44, 36
		};
	//Standard Table
	static int pc1_Dtable[28] = {
		63, 55, 47, 39, 31, 23, 15,
		 7, 62, 54, 46, 38, 30, 22,
		 14, 6, 61, 53, 45, 37, 29,
		 21, 13,  5, 28, 20, 12, 4
		};
	//every sequence round count have different value
	//key num = feistel structure repeat count
	static int round_table[16] = {
		1, 1, 2, 2,
		2, 2, 2, 2,
		1, 2, 2, 2,
		2, 2, 2, 1
	};
	//Standard Table (56bit -> 48bit)
	static int pc2_table[48] = {
		14, 17, 11 , 24, 1,  5,  3, 28,
		15,  6, 21, 10, 23, 19, 12,  4,
		26,  8, 16,  7, 27, 20, 13,  2,
		41, 52, 31, 37, 47, 55, 30, 40,
		51, 45, 33, 48, 44, 49, 39, 56,
		34, 53, 46, 42, 50, 36, 29, 32
		};
	
	uint64_t result = 0, temp = 0;
	uint32_t C = 0, D = 0;

	temp = DES::load64(key);

	//PC - 1
	for(int i = 0; i < 28; i++)
	{
		C |= ((temp >> (64 - pc1_Ctable[i])) & 1U) << (27 - i);
		D |= ((temp >> (64 - pc1_Dtable[i])) & 1U) << (27 - i);
	}
	
	for(int i = 0; i < 16; i++)
	{
		//Left Circular Shift
		C = LCS28(C, round_table[i]);
		D = LCS28(D, round_table[i]);
	
		temp = static_cast<uint64_t>(D);
		temp |= static_cast<uint64_t>(C) << 28;	

		//PC - 2
		for(int j = 0; j < 48; j++)
			result |= ((temp >> (56 - pc2_table[j])) & 1ULL) << (47 - j);

		//save sub key
		this->EnsubKey[i] = result;
		this->DesubKey[15 - i] = result;

		result = 0;
	}

}

uint64_t DES::FP(uint64_t msg)
{
	//Stanard FP Table (inverse metrix of IP table)
	static int table[64] = {
		40, 8, 48, 16, 56, 24, 64, 32,
		39, 7, 47, 15, 55, 23, 63, 31,
		38, 6, 46, 14, 54, 22, 62, 30,
		37, 5, 45, 13, 53, 21, 61, 29,
		36, 4, 44, 12, 52, 20, 60, 28,
		35, 3, 43, 11, 51, 19, 59, 27,
		34, 2, 42, 10, 50, 18, 58, 26,
		33, 1, 41,  9, 49, 17, 57, 25
		};
	
	uint64_t result = 0;
        
	for(int i = 0; i < 64; i++)
		result |= ((msg >> (64 - table[i])) & 1ULL) << (63 - i);
        
	return result;				
}

void DES::cipher(const block input, block output)
{
	if(input == nullptr ||
	output == nullptr)
		return;
	uint64_t input64 = load64(input);

	//Initailze Permutation
	input64 = this->IP(input64);

	//feistel structure
	input64 = feistel::round(input64, EnsubKey);

	//Final Permutation
	input64 = this->FP(input64);

	//store output for uint8+t array
	store64(input64, output);
}

void DES::decipher(const block input, block output)
{
	if(input == nullptr ||
	output == nullptr)
		return;
	uint64_t input64 = load64(input);

	//Initailze Permutation
	input64 = this->IP(input64);

	//feistel structure
	input64 = feistel::round(input64, DesubKey);

	//Final Permutation
	input64 = this->FP(input64);

	store64(input64, output);
}

size_t DES::get_block_size(void) const noexcept { return block_size; }

bool DES::chkParity(const block key)
{
	for(size_t i = 0; i < 8; i++)
	{
		uint8_t group = key[i];

		group ^= group >> 4;
		group ^= group >> 2;
		group ^= group >> 1;
		
		if((group & 0x01) == 0x00)
        		return false;
	}

	return true;
}

void DES::setParity(block key)
{
	for(size_t i = 0; i < 8; i++)
        {
                uint8_t group = key[i];

                group ^= group >> 4;
                group ^= group >> 2;
                group ^= group >> 1;

                if((group & 0x01) == 0x00)
                        key[i] ^= 0x01;
        }

}
