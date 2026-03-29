#include <fstream>
#include <random>
#include <iostream>
#include <cstring>

int main(int argc, char* argv[]) {
	if(argc == 1)
		return -1;
	uint64_t size = static_cast<uint64_t>(atoi(argv[1]));
	char file_name[30] = "random_";
	
	std::strncat(file_name, argv[1], std::strlen(argv[1]));
	std::strncat(file_name, "MB.bin\0", sizeof("MB.bin\0"));

    std::ofstream out(file_name, std::ios::binary);
    std::mt19937_64 rng(std::random_device{}());
	
    const size_t SIZE = size * 1024 * 1024;
    for (size_t i = 0; i < SIZE; i++) {
        char byte = rng() % 256;
        out.write(&byte, 1);
    }
	out.close();

	return 0;
}
