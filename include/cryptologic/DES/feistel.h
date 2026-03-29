#pragma once
#include <cstdint>
#include "../console/iprogress.h"
#include <vector>

class feistel{
private:
//key scheduled for 16 -> feistel cac 16 times
	static constexpr int repeat = 16;
//diffusion & confusion & key mix => main function
	uint32_t F(uint32_t, uint64_t);
protected:
	feistel() {};
	~feistel(){};

	IProgress* m_callback = nullptr;

	uint64_t round(uint64_t, std::vector<uint64_t>&);

public:
	void setProgressCallback(IProgress* callback) { m_callback = callback; }
};

