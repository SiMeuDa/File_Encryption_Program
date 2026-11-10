#include "interface/menu.h"
#include "cryptologic/HASH/HASH.h"
#include "cryptologic/HASH/HES/HES.h"
#include <iostream>
#include <cstdint>
#include <cctype>
#include <cstring>
#include <string>
#include <string_view>
#include <limits>
#if __cplusplus >= 201703L
#include <filesystem>
#endif

void menu::main(int& choice)
{
	std::cout << "========== File En/Decryption Program ==========\n";
	std::cout << "1. Encrypt File\n";
	std::cout << "2. Decrypt File\n";
	std::cout << "8. Settings\n";
	std::cout << "9. Terminate Program\n";
	std::cout << "===============================================\n";

	safe_input(choice);

}

void menu::take_key(const crypto::crypto_logic& cl, block& key)
{
	std::string str = "";
	bool eof = false;
	HASH* h = nullptr;
        h = new HES(cl);

	size_t repeat = 0;

	if(cl == crypto::crypto_logic::DES)
		repeat = 1;
	else if(cl == crypto::crypto_logic::Triple_DES)
		repeat = 2;
	else{
		delete h;
		h = nullptr;

		throw std::invalid_argument("invalid crypto logic");
	}	
	

	if(key == nullptr)
	{
		key = new(std::nothrow) uint8_t[8 * repeat];
		if(key == nullptr)
		{
			delete h;
			h = nullptr;

			throw std::bad_alloc();
		}
	}

	std::cin.ignore();
	std::cout << "key: ";
	
	getline(std::cin, str);
	if(str != "file")
		h->hash(std::string_view(str), key);
#if __cplusplus >= 201703L
	else
	{
		std::cout << "file path: ";
		getline(std::cin, str);

		h->hash(std::filesystem::path(str), key);
	}
#endif

	delete h;
	h = nullptr;
}

void menu::crypto_logic(int& choice)
{

	std::cout << ANSI::SCR_RESET << ANSI::CUR_HOME << std::endl;

	std::cout << "============= Select Crypto Logic ==============\n";
	std::cout << "1. DES\n";
	std::cout << "2. Triple DES\n";
	std::cout << "3. AES\n";
	std::cout << "9. Before Menu\n";
	std::cout << "===============================================\n";

	safe_input(choice);

}
	
void menu::op_mode(int& choice)
{
	
	std::cout << ANSI::SCR_RESET << ANSI::CUR_HOME << std::endl;

	std::cout << "============ Select Operation Mode =============\n";
	std::cout << "1. Electric CodeBook mode\n";
	std::cout << "2. Cipher Block Chaining mode\n";
	std::cout << "3. Cipher FeedBack mode\n";
	std::cout << "4. Output FeedBack mode\n";
	std::cout << "5. CounTeR mode\n";
	std::cout << "9. Before_Menu\n";
	std::cout << "===============================================\n";

	safe_input(choice);
}

void menu::setting(int& choice)
{
	
	std::cout << "=================== Settings ===================\n";
	std::cout << "1. Set New File Path\n";
	std::cout << "2. Show set File Path\n";
	std::cout << "3. Diagonsis Crypto Logic\n";
	std::cout << "4. Program Information\n";
	std::cout << "9. Back to Main Menu\n";
	std::cout << "===============================================\n";
	
	safe_input(choice);

}

void menu::information(void)
{
	std::cout << std::endl;
	std::cout << "================== Information =================\n";
	std::cout << "Made by\t\tYesung Kwon\n";
	std::cout << "Coded by\tC++ language(17)\n";
	std::cout << "Version\t\t1.1.1\n";
	std::cout << "================================================\n";
	std::cout << std::endl;
}

void menu::safe_input(int& choice)
{
	//if cin take invalid value, clear buffer and take value again
	do{
		std::cout << "Choice: ";
		std::cin >> choice;

		if(!std::cin.fail()){
			break;
		}else{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

			MESSAGE("Invalid Input", -1);
	}}while(true);
}

void menu::MESSAGE(const char* msg, int code)
{
	if(code == 0)
	{
		std::cout << ANSI::BLUE << "[SYSTEM]: Service Under Preparation" << ANSI::COLOR_RESET << std::endl;
	}
	else if(code == 1)
	{
		std::cout << ANSI::BLUE << "[SYSTEM]: Success to " << msg << ANSI::COLOR_RESET << std::endl;
	}
	else if(code == -1)
	{
		std::cerr << ANSI::RED << "[SYSTEM]: ERROR OCCUR" << std::endl;
		std::cerr << "[ERROR MSG]: " << msg << ANSI::COLOR_RESET << std::endl;
	}

}

void menu::MESSAGE(std::exception& e)
{
	std::cerr << ANSI::RED << "[SYSTEM]: ERROR OCCUR" << std::endl;
	std::cerr << "[ERROR MSG]: " << e.what() << ANSI::COLOR_RESET << std::endl;
	

}

void menu::loading(std::streamsize max_size, std::streamsize now_size)
{
	
	//erase terminal
	std::cout << menu::ANSI::SCR_RESET << menu::ANSI::CUR_HOME;
	
	std::cout << ANSI::YELLOW << "=================================" << ANSI::COLOR_RESET << std::endl;
	std::cout << ANSI::YELLOW << "      Loading...( " << (static_cast<double>(now_size) / max_size) * 100
	       	<< "%)" << ANSI::COLOR_RESET << std::endl;
	std::cout << ANSI::YELLOW << "=================================" << ANSI::COLOR_RESET << std::endl;

	if(max_size <= now_size)
	{
		//erase terminal
		std::cout << ANSI::SCR_RESET << ANSI::CUR_HOME;
		
		std::cout << ANSI::GREEN << "=================================" << ANSI::COLOR_RESET << std::endl;
		std::cout << ANSI::GREEN << "    Complete Crypto Logic... " << ANSI::COLOR_RESET << std::endl;
		std::cout << ANSI::GREEN << "=================================" << ANSI::COLOR_RESET << std::endl << std::endl;
	}
}
