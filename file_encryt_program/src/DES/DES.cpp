#include "DES.h"
#include <cstdint>
#include <vector>
#include <string>
#include <bitset>

bool DES::chkParity(uint64_t key)
{
	for (int i = 0; i < 8; i++)
    {
    	uint8_t group = (key >> (i * 8)) & 0xFF;
		//count 8bit's 1
 	    int ones = std::bitset<8>(group).count();
		//if count odd, it is ok		
       	bool parity = (ones % 2 == 1) ? 1 : 0;

       if(parity == 0)
		   return false;
    }

	return true;

}

uint32_t DES::LCS(uint32_t value, size_t count)
{
	for(size_t i = 0; i < count; i++)
	{
		uint32_t msb = (value >> 27) & 1U;
		
		value = (value << 1) & 0xFFFFFFFU;
		
		value |= msb;
	}
	return value;
}

uint64_t DES::IP(uint64_t msg)
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
	uint64_t result = 0;

	for(int i = 0; i < 64; i++)
		result |= ((msg  >> (64 - table[i])) & 1ULL) << (63 - i);

	return result;
}

bool DES::keySchedule(uint64_t key)
{
	//check Parity Bit
	if(!chkParity(key))
		return false;
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
	uint64_t result = 0;
	uint64_t temp = 0;
	uint32_t C = 0, D = 0;

	//PC - 1
	for(int i = 0; i < 28; i++)
	{
		C |= ((key >> (64 - pc1_Ctable[i])) & 1U) << (27 - i);
		D |= ((key >> (64 - pc1_Dtable[i])) & 1U) << (27 - i);
	}
	
	for(int i = 0; i < 16; i++)
	{
		//Left Circular Shift
		C = LCS(C, round_table[i]);
		D = LCS(D, round_table[i]);

		temp = static_cast<uint64_t>(D);
		temp |= (static_cast<uint64_t>(C)) << 28;

		//PC - 2
		for(int j = 0; j < 48; j++)
			result |= ((temp >> (56 - pc2_table[j])) & 1ULL) << (47 - j);

		//save sub key
		this->subKey[i] = result;
		
		result = 0;
	}

	return true;
}

uint64_t DES::FP(uint64_t msg)
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
				
	uint64_t result = 0;

	for(int i = 0; i < 64; i++)
		result |= ((msg  >> (64 - table[i])) & 1ULL) << (63 - i);


	return result;
}

uint64_t DES::cipher(uint64_t msg, uint64_t key)
{
	//Scheduling Key to sub Key
	if(!this->keySchedule(key))
		return 0;
	//Initailze Permutation
	msg = this->IP(msg);
	//feistel structure
	msg = round(msg, this->subKey);
	//Final Permutation
	msg = this->FP(msg);

	return msg;
}

uint64_t DES::decipher(uint64_t msg, uint64_t key)
{
	
	//Scheduling Key to sub Key
	if(!this->keySchedule(key))
		return 0;
	//Initailze Permutation
	msg = this->IP(msg);

	std::vector<uint64_t> DesubKey;
	DesubKey.resize(16);
	for(int i = 0; i < 16; i++)
		DesubKey.at(i) = subKey.at(15 - i);
	
	//feistel structure
	msg = round(msg, DesubKey);

	//Final Permutation
	msg = this->FP(msg);

	return msg;
}
