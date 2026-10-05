#include "interface/menu.h"
#include <iostream>
#include <cstdint>
#include <cctype>
#include <cstring>
#include <string>
#include <string_view>
#include <limits>

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

void menu::take_hex(block key)
{
	std::string raw_buffer, buffer;

	bool isHex = false;
	
	do{
		std::cout << "Key Input(0x~): ";
	
		std::getline(std::cin, raw_buffer);

		if(raw_buffer.find("0x") == 0)
		{
			buffer = raw_buffer.substr(2);

			if(buffer.length() == 16)
			{
				for(size_t i = 0; i < 16; i++)
				{
					if(!isxdigit(buffer[i]))
					{
						std::cout << "[ERROR]: Invalid Key Value (key contain non-hex value)" << std::endl;
						isHex = false;
						break;
					}else
						isHex = true;
				}
			}
			else{
				std::cout << "[ERROR]: Invalid key value (input over key size)" << std::endl;
			}
		}else{
			std::cout << "[ERROR]: Invalid Key Value (key don't start with 0x)" << std::endl;
		}
	}while(!isHex);

	auto to_hex = [](char c) -> uint8_t {
        	if (c >= '0' && c <= '9') return c - '0';
		else if (c >= 'A' && c <= 'F') return c - 'A' + 10;
		else if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        	return 0;
  	  };
	
	for(int i = 0; i < 8; i++)
	{
		key[i] = to_hex(buffer[2 * i]) << 4;
		key[i] |= to_hex(buffer[2 * i + 1]);
	}
}

void menu::take_key(crypto::crypto_logic cl, block& key)
{
	block key_buffer = new uint8_t[8];

	size_t repeat = 0;

	if(cl == crypto::crypto_logic::DES)
		repeat = 1;
	else if(cl == crypto::crypto_logic::Triple_DES)
		repeat = 2;
	else
		return;
	
	key = new uint8_t[8 * repeat];

	for(size_t i = 0; i < repeat; i++)
	{
		take_hex(key_buffer);

		std::memcpy(key + (8 * i), key_buffer, 8);
	}

	delete[] key_buffer;
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
	std::cout << "================== Information =================\n";
	std::cout << "Made by\tYesung Kwon\n";
	std::cout << "Coded by\tC++ language(17)\n";
	std::cout << "Version\t1.0.1\n";
	std::cout << "================================================\n";
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
