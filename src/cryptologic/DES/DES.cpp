#include "cryptologic/DES/DES.h"
#include "cryptologic/crypto.h"
#include <cstdint>
#include <vector>
#include <algorithm>

DES::DES(BLOCK key)
{
	setParity(key);
	if(!chkParity(key))
		throw std::invalid_argument("Invalid Key Value");

	EnsubKey.resize(16);
	DesubKey.resize(16);

	this->keySchedule(key);
}

DES::~DES() { crypto::secure_erase(EnsubKey); crypto::secure_erase(DesubKey); }

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


BLOCK DES::IP(BLOCK msg)
{
        //Standard IP Table
        int table[64] = {
                58, 50, 42, 34, 26, 18, 10, 2,
                60, 52, 44, 36, 28, 20, 12, 4,
                62, 54, 46, 38, 30, 22, 14, 6,
                64, 56, 48, 40, 32, 24, 16, 8,
                57, 49, 41, 33, 25, 17,  9, 1,
                59, 51, 43, 35, 27, 19, 11, 3,
                61, 53, 45, 37, 29, 21, 13, 5,
                63, 55, 47, 39, 31, 23, 15, 7
                };
        
	BLOCK result(block_size, std::byte{0x00});
        std::byte change_block;
        
	for(size_t i = 0; i < block_size; i++){
                for(size_t j = 0; j < 8; j++){
                        size_t index = i * block_size + j;
                        int bit_pos = table[index] - 1;

                        change_block = msg[bit_pos / 8];

                        change_block = change_block >> (7 - (bit_pos % 8)) & std::byte{0x01};

                        result[i] |= change_block << (7 - j);
        }}
        return result;
}

void DES::keySchedule(BLOCK key)
{
	//Standard Table
	int pc1_Ctable[28] = {
		57, 49, 41, 33, 25, 17,  9,
		 1,	58, 50, 42, 34,	26, 18,
		10,  2,	59, 51, 43, 35, 27,
		19, 11,  3,	60, 52, 44, 36
		};
	//Standard Table
	int pc1_Dtable[28] = {
		63, 55, 47, 39, 31, 23, 15,
		 7, 62, 54, 46, 38, 30, 22,
		 14, 6, 61, 53, 45, 37, 29,
		 21, 13,  5, 28, 20, 12, 4
		};
	//every sequence round count have different value
	//key num = feistel structure repeat count
	int round_table[16] = {
		1, 1, 2, 2,
		2, 2, 2, 2,
		1, 2, 2, 2,
		2, 2, 2, 1
	};
	//Standard Table (56bit -> 48bit)
	int pc2_table[48] = {
		14, 17, 11 , 24, 1,  5,  3, 28,
		15,  6, 21, 10, 23, 19, 12,  4,
		26,  8, 16,  7, 27, 20, 13,  2,
		41, 52, 31, 37, 47, 55, 30, 40,
		51, 45, 33, 48, 44, 49, 39, 56,
		34, 53, 46, 42, 50, 36, 29, 32
		};
	
	uint64_t result = 0, temp = 0;
	uint32_t C = 0, D = 0;

	temp = block::to_integer<uint64_t>(key);

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
	
		temp = static_cast<uint64_t>(C);
		temp |= static_cast<uint64_t>(D) << 28;	

		//PC - 2
		for(int j = 0; j < 48; j++)
			result |= ((temp >> (56 - pc2_table[j])) & 1ULL) << (47 - j);

		//save sub key
		this->EnsubKey[i] = result;
		this->DesubKey[15 - i] = result;

		result = 0;
	}
}

BLOCK DES::FP(BLOCK msg)
{
	//Stanard FP Table (inverse metrix of IP table)
	int table[64] = {
		40, 8, 48, 16, 56, 24, 64, 32,
		39, 7, 47, 15, 55, 23, 63, 31,
		38, 6, 46, 14, 54, 22, 62, 30,
		37, 5, 45, 13, 53, 21, 61, 29,
		36, 4, 44, 12, 52, 20, 60, 28,
		35, 3, 43, 11, 51, 19, 59, 27,
		34, 2, 42, 10, 50, 18, 58, 26,
		33, 1, 41,  9, 49, 17, 57, 25
		};
	
	BLOCK result(block_size, std::byte{0x00});
        std::byte change_block;
        
	for(size_t i = 0; i < block_size; i++){
                for(size_t j = 0; j < 8; j++){
                        size_t index = i * block_size + j;
                        int bit_pos = table[index] - 1;

                        change_block = msg[bit_pos / 8];

                        change_block = change_block >> (7 - (bit_pos % 8)) & std::byte{0x01};

                        result[i] |= change_block << (7 - j);
        }}
        
	return result;				
}

BLOCK DES::cipher(BLOCK org_msg)
{
	//vector size check
	if(org_msg.size() != block_size)
	{
		throw std::length_error("Invalid Block Size");
	}

	//Initailze Permutation
	org_msg = this->IP(org_msg);

	//feistel structure
	org_msg = block::to_block(round(block::to_integer<uint64_t>(org_msg), EnsubKey));

	//Final Permutation
	org_msg = this->FP(org_msg);

	return org_msg;
}

BLOCK DES::decipher(BLOCK org_msg)
{
	//vector size check
	if(org_msg.size() != block_size)
	{
		throw std::length_error("Invalid Block Size");
	}

	//Initailze Permutation
	org_msg = this->IP(org_msg);

	//feistel structure
	org_msg = block::to_block(round(block::to_integer<uint64_t>(org_msg), DesubKey));

	//Final Permutation
	org_msg = this->FP(org_msg);

	return org_msg;
}

size_t DES::get_block_size(void) { return block_size; }

bool DES::chkParity(const BLOCK& key)
{
	for(size_t i = 0; i < key.size(); i++)
	{
		std::byte group = key[i];

		group ^= group >> 4;
		group ^= group >> 2;
		group ^= group >> 1;
		
		if((group & std::byte{0x01}) == std::byte{0x00})
        		return false;
	}

	return true;
}

bool DES::setParity(BLOCK& key)
{
	for(size_t i = 0; i < key.size(); i++)
        {
                std::byte group = key[i];

                group ^= group >> 4;
                group ^= group >> 2;
                group ^= group >> 1;

                if((group & std::byte{0x01}) == std::byte{0x00})
                        	key[i] ^= std::byte{0x01};
        }

        return true;
}
