#include <fstream>
#include <random>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
using namespace std::string_view_literals;

struct ANSI
{
        ANSI() = delete;
        //COLOR 
        static constexpr std::string_view COLOR_RESET   = "\033[0m";
        static constexpr std::string_view BLACK         = "\033[30m";
        static constexpr std::string_view RED           = "\033[31m";
        static constexpr std::string_view GREEN         = "\033[32m";
        static constexpr std::string_view YELLOW        = "\033[33m";
        static constexpr std::string_view BLUE          = "\033[34m";
        static constexpr std::string_view MAGENTA       = "\033[35m";
        static constexpr std::string_view CYAN          = "\033[36m";

        //Screen & Cursor
        static constexpr std::string_view SCR_RESET     = "\033[2J";
        static constexpr std::string_view CUR_HOME      = "\033[H";
};

void loading(size_t, size_t);
bool isVtype(char**, std::string&, size_t&, size_t&);

int main(int argc, char* argv[]) 
{
	std::string file_name = "random_";
	size_t buffer = 1, size = 0;


	if(argc != 3 || !isVtype(argv, file_name, size, buffer))
	{
		std::cerr << ANSI::BLUE << "[Usage]: ./[file_name] -[B/KB/MB/GB] [size]" << ANSI::COLOR_RESET << std::endl;
		return -1;
	}

	//open file
	std::ofstream out(file_name, std::ios::binary);
	if(out.fail())
	{
		std::cerr << ANSI::RED << "[ERROR]: Failed to open file" << ANSI::COLOR_RESET << std::endl;
		return -1;
	}
	//random number setting
    	std::mt19937_64 rng(std::random_device{}());

    	for (size_t i = 0; i < size; i++) 
	{
		loading(size, i);
		for(size_t j = 0; j < buffer; j++)
		{
        		char byte = rng() % 256;
        		out.write(&byte, 1);
		}
    	}
	
	loading(size, size);

	out.close();

	return 0;
}

bool isVtype(char** argv, std::string& file, size_t& size, size_t& buffer)
{
	
	buffer = 1;

	std::string argument = argv[2];
	const std::string Vtype[4] = { "-B", "-KB", "-MB", "-GB"};
	try{
		size = stoi(argument);
		if(size > 1024)
			throw std::invalid_argument("Key value is Too Big");
	}catch(std::exception& e){
		return false;
	}
	
	argument = argv[1];

	for(int i = 0; i < 4; i++)
	{
		if(argument.compare(Vtype[i]) == 0)
		{
			file.append(std::to_string(size)).append(argument.begin() + 1, argument.end()).append(".bin");
			for(int j = 0; j < i; j++)
				buffer *= 1024;
			return true;
		}
	}

	return false;

}

void loading(size_t total, size_t now)
{
	double percent = static_cast<double>(now);
	percent = percent * 100.0 / total;
	

	std::clog << ANSI::CUR_HOME << ANSI::SCR_RESET;
	if(total != now)
	{
		std::clog << ANSI::YELLOW;
	        std::clog << "==============================" << std::endl;
		std::clog << "Loading... (" << percent << "%)" << std::endl;
		std::clog << "==============================" << std::endl;
	}
	else
	{
		std::clog << ANSI::GREEN;
	        std::clog << "==============================" << std::endl;
		std::clog << "Success to generate File" << std::endl;
		std::clog << "==============================" << std::endl;
	}	
	std::clog << ANSI::COLOR_RESET << std::endl;
}
