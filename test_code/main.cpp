#include "cryptologic/crypto.h" 
#include "cryptologic/DES/DES.h"
#include "cryptologic/mode/mode.h"
#include "cryptologic/mode/CBC.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <filesystem>
#include <string>
#include <stdexcept>
#include <memory>
namespace fs = std::filesystem;

enum class menuOption { 
	MAIN = 0, CRYPTO_LOGIC, OP_MODE,
	SETTING = 9 
};
enum class cryptoLogic {
	DES = 0, Triple_DES, AES
};
bool setPath(fs::path& p);
void Menu(int& choice, menuOption option);

int main(void)
{
	fs::path p;
	int choice = 0;
	uint64_t key = 0, key2 = 0;
	bool isSetFile = false, isEncryption = true;
	mode* op_mode;
	cryptoLogic cLog;
#ifdef LOG
	std::clog << "[SYSTEM] Start File En/Decryption Program" << std::endl;
	std::clog << "[SYSTEM] Checking En/Decryption Logic" << std::endl;
#endif
	//Add Crypto Logic Checking using log
	
	
	while(true)
	{
		Menu(choice, menuOption::MAIN);

		if((choice == 1) or (choice == 2))
		{
		//En/Decryption on path file
			if(!isSetFile){
				std::cout << "[ERROR] Please set file path on Settings(8)" << std::endl;
				continue;
			}
		
			if(choice == 1)
				isEncryption = true;
			else if(choice == 2)
				isEncryption = false;
			//set Crypto Logic(DES, Triple DES, AES, etc..)
			Menu(choice, menuOption::CRYPTO_LOGIC);

			cLog = static_cast<cryptoLogic>(choice - 1);
			//set Key Logic
			std::cout << "Key Input: ";
			std::cin >> key;
			if(cLog == cryptoLogic::Triple_DES){
				std::cout << "Key 2 Input: ";
				std::cin >> key2;
			}

			Menu(choice, menuOption::OP_MODE);


			if(isEncryption)
			{
				std::cout << "DO Encryption" << std::endl;
			}
			else
			{
				std::cout << "DO Decryption" << std::endl;
			}

		}else if(choice == 8){
		//Setting
			Menu(choice, menuOption::SETTING);
			
			if(choice == 1){
				if(!isSetFile){
					std::cout << "[ERROR] There is no set file path";
				}else{
					std::cout << "Now Set File Path: " << fs::canonical(p);
				}
				std::cout << std::endl;
			}else if(choice == 2){
			//Setting file path
				while(!setPath(p));
				isSetFile = true;
			
			}else if(choice == 3){
			//Diagonsis Crypto Logic
#ifdef LOG
				std::clog << "[SYSTEM] On Programming" << std::endl;
#endif
			}else if(choice == 4){
			//print out program information			
#ifdef LOG
				std::clog << "[SYSTEM] On Programming" << std::endl;
#endif
			}else if(choice != 9){
				//to do not disturb printing main menu, use cout(not cerr)
				std::cout << "[ERROR] Please Select Number On Menu" << std::endl;
			}
		}else if(choice == 9){
		//Terminate Program
			std::cout << "[SYSTEM] Terminate Program...\n" << std::endl;
			break;
		}else{
			//to do not disturb printing main menu, use cout(not cerr)
			std::cout << "[ERROR] Please Select Number On Menu" << std::endl;
		}
	}

	return 0;
}

bool setPath(fs::path& p)
{
//change to show saved path and add or delete path
	std::string path_str;
	std::cout << "File Path: ";

	std::cin >> path_str;
	p = path_str;

	try{
		//check existence, isFile, isDirectory
		if(!fs::exists(p)){
			throw std::runtime_error("File does not exists");
		}else if(!fs::is_regular_file(p)){
			throw std::runtime_error("File is not regular file");
		}else if(fs::is_directory(p)){
			throw std::runtime_error("Directory can't be En/Decrypted");
		}
	}catch(std::exception& e){
		std::cerr << "[ERROR] " << e.what() << std::endl << std::endl;
		return false;
	}
	std::cout << "\n[SYSTEM] Success To Setting File Path\nSet Path: " << fs::canonical(p) << "\n";
	//Normal return
	return true;
}

void Menu(int& choice, menuOption option)
{
	std::cout << std::endl;
	if(option == menuOption::MAIN)
	{
		std::cout << "========== File En/Decryption Program ==========\n";
		std::cout << "1. Encrypt File\n";
		std::cout << "2. Decrypt File\n";
		std::cout << "8. Settings\n";
		std::cout << "9. Terminate Program\n";
	}else if(option == menuOption::CRYPTO_LOGIC){
		std::cout << "============= Select Crypto Logic ==============\n";
		std::cout << "1. DES\n";
		std::cout << "2. Triple DES\n";
		std::cout << "3. AES\n";
	}else if(option == menuOption::OP_MODE)
	{
		std::cout << "============ Select Operation Mode =============\n";
		std::cout << "1. Electric CodeBook mode\n";
		std::cout << "2. Cipher Block Chaining mode\n";
		std::cout << "3. Cipher FeedBack mode\n";
		std::cout << "4. Output FeedBack mode\n";
		std::cout << "5. CounTeR mode\n";

	}else if(option == menuOption::SETTING)
	{
		std::cout << "=================== Settings ===================\n";
		std::cout << "1. Show Now File Path\n";
		std::cout << "2. Set New File Path\n";
		std::cout << "3. Diagonsis Crypto Logic\n";
		std::cout << "4. Program Information\n";
		std::cout << "9. Back to Main Menu\n";
	}

	std::cout << "===============================================\n";
	std::cout << "Choice: ";
	
	//if cin take invalid value, clear buffer and take value again
	do{
		std::cin >> choice;
		if(!std::cin.fail()){
			break;
		}else{
			std::cin.clear();
		    	std::fseek(stdin, 0, SEEK_END);
	}}while(true);

	std::cout << std::endl;	
	return;
}
