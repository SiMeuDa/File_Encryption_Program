#ifndef FEISTEL_H
#define FEISTEL_H
#include <cstdint>
#include <array>

class feistel{
private:
//key scheduled for 16 -> feistel cac 16 times
	static constexpr int repeat = 16;
	inline static bool isFirst = true;
	inline static uint32_t SP_table[8][64];

//set S-P table
	void setSP(void);

//diffusion & confusion & key mix => main function
	static uint32_t F(uint32_t, uint64_t);

	static uint32_t chg_F(uint32_t, uint64_t);
protected:
	feistel(){ if(isFirst) setSP(); }
	~feistel(){}


	static uint64_t round(uint64_t, const std::array<uint64_t, repeat>&);

};
#endif
