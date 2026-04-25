#include <fstream>
#include <random>
#include <iostream>
#include <cstring>
#include <string_view>
using namespace std::string_view_literals;

int main(int argc, char* argv[]) {
	if(argc != 3)
	{
		std::cerr << "[Usage]: ./[file_name] -[B/KB/MB/GB] [size]" << std::endl;
		return -1;
	}
	
	uint16_t repeat = 0;
	char file_name[30] = "random_";
	char size_name[10] = "";

	if(argv[1] == "-B"sv)
		std::strncat(size_name, "B.bin\0", sizeof("B.bin\0"));
	else if(argv[1] == "-KB"sv)
	{
		repeat = 1;
		std::strncat(size_name, "KB.bin\0", sizeof("KB.bin\0"));
	}
	else if(argv[1] == "-MB"sv)
	{
		repeat = 2;
		std::strncat(size_name, "MB.bin\0", sizeof("MB.bin\0"));
	}
	else if(argv[1] == "-GB"sv)
	{
		repeat = 3;
		std::strncat(size_name, "GB.bin\0", sizeof("GB.bin\0"));
	}
	else{
		std::cerr << "[Usage]: ./[file_name] -[B/KB/MB/GB] [size]" << std::endl;
		return -1;
	}

	uint64_t size = static_cast<uint64_t>(atoi(argv[2]));
	
	//make name
	std::strncat(file_name, argv[2], std::strlen(argv[2]));
	std::strncat(file_name, size_name, std::strlen(size_name));
	
	//open file
	std::ofstream out(file_name, std::ios::binary);
	//random number setting
    std::mt19937_64 rng(std::random_device{}());

	//set size
	for(uint16_t i = 0; i < repeat; i++)
		size *= 1024;

    for (size_t i = 0; i < size; i++) 
	{
        char byte = rng() % 256;
        out.write(&byte, 1);
    }
	out.close();

	return 0;
}
