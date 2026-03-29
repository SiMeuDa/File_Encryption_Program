#pragma once
#include "DES/DES.h"
#include <cstdint>
#include <vector>
#include <string>

class mode : public DES{
private:
	constexpr static int block_len = 8;
	//Secure memset
	void secure_wipe(void*, size_t);
	//padding: PKCS#7 Standard
	//do padding
	std::string padding(std::string);
	//after unpadding size
	size_t real_size(const std::vector<uint64_t>& msg);
	//return random value(64bit size)
	inline uint64_t random(void);

	//change msg to integer msg(vector)
	std::vector<uint64_t> to_integer(std::string);
	//change integer msg(vector> to msg
	std::string from_integer(std::vector<uint64_t>);
protected:
	mode()	{};
	~mode() {};
	//Electric CodeBook mode
	std::vector<uint64_t> ECB(std::string, uint64_t&, uint64_t&);
	std::string ECB(std::vector<uint64_t>, uint64_t&, uint64_t&);
	//Cipher Block Chaining mode
	std::vector<uint64_t> CBC(std::string, uint64_t&, uint64_t&);
	std::string CBC(std::vector<uint64_t>, uint64_t&, uint64_t&);
	//Cipher FeedBack mode
	std::vector<uint64_t> CFB(std::string, uint64_t&, uint64_t&);
	std::string CFB(std::vector<uint64_t>, uint64_t&, uint64_t&);
	//Output FeedBack mode
	std::vector<uint64_t> OFB(std::string, uint64_t&, uint64_t&);
	std::string OFB(std::vector<uint64_t>, uint64_t&, uint64_t&);
	//CounTeR
	std::vector<uint64_t> CTR(std::string, uint64_t&, uint64_t&);
	std::string CTR(std::vector<uint64_t>, uint64_t&, uint64_t&);

};
