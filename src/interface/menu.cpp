#include "interface/menu.h"
#include <iostream>
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
	std::cout << "Version\t1.0.0\n";
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
