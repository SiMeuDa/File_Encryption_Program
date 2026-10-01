#ifndef FEISTEL_H
#define FEISTEL_H
#include <cstdint>
#include <vector>

class feistel{
private:
//key scheduled for 16 -> feistel cac 16 times
	static constexpr int repeat = 16;
//diffusion & confusion & key mix => main function
	inline uint32_t F(uint32_t, uint64_t);
protected:
	feistel() {};
	~feistel(){};


	uint64_t round(uint64_t, std::vector<uint64_t>&);

};
#endif
