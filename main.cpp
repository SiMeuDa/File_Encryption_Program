//Base Header
#include "cryptologic/crypto.h" 
//Crypto Logic
#include "cryptologic/DES/DES.h"
//#include "cryptologic/DES/Triple_DES.h"
//#incldue "cryptologic/AES/AES_128.h"
//operation modes
#include "cryptologic/mode/mode.h"
#include "cryptologic/mode/CTR.h"
//muti-threading queue
#include "interface/thread_queue.h"
#include <thread>
#include <future>
//STDIO
#include <iostream>
//file IO class
#include <fstream>
#include <filesystem>
#include <cstring>
//data manipulate class(for API input)
#include <vector>
#include <cstdint>
#include <array>
//user input
#include <string>
#include "interface/menu.h"
//exception handling
#include <stdexcept>
#include <memory>
#include <functional>

//for debug log
#ifdef LOG
void log(const char* msg){ std::clog << "[SYSTEM]: " << msg << std::endl;}
#endif

namespace fs = std::filesystem;

bool setPath(fs::path& p);
bool do_stream(fs::path, std::function<BLOCK(const BLOCK&)>);

int main(void)
{
	fs::path p;
	menu m;
	int choice = 0;
	uint64_t int_key = 0, int_key2 = 0;
	BLOCK key;
	bool isSetFile = false, isEncryption = true;
	mode* op_mode;
	crypto::crypto_logic cLog;
#ifdef LOG
	log("Start File En/Decryption Program");
#endif
	
	while(true)
	{
		m.main(choice);

		if((choice == 1) or (choice == 2))
		{
		//En/Decryption on path file
			if(!isSetFile){
				m.MESSAGE("Please set file path on Settings(8)", -1);
				continue;
			}
		
			if(choice == 1)
				isEncryption = true;
			else if(choice == 2)
				isEncryption = false;
			//set Crypto Logic(DES, Triple DES, AES, etc..)
			m.crypto_logic(choice);
			
			if(choice > static_cast<int>(crypto::crypto_logic::END) 
			|| choice < static_cast<int>(crypto::crypto_logic::DES))
			{
				m.MESSAGE("Invalid Input", -1);
				continue;
			}
			
			cLog = static_cast<crypto::crypto_logic>(choice - 1);

			//set Key Logic
#ifdef LOG
			log("Start to get key");
#endif
			std::cout << "Key Input: ";
			std::cin >> int_key;

			key = DES::to_block(int_key);

			if(cLog == crypto::crypto_logic::Triple_DES){
				std::cout << "Key 2 Input: ";
				std::cin >> int_key2;
				BLOCK key_app = DES::to_block(int_key2);
				
				key.insert(key.end(), key_app.begin(), key_app.end());
			}
#ifdef LOG
			log("Success to get and interpret key");
#endif
			//set operation mode
			m.op_mode(choice);
			
			if(choice == 1){
				m.MESSAGE();
				continue;
			}else if(choice == 2){
				m.MESSAGE();
				continue;
			}else if(choice == 3){
				m.MESSAGE();
				continue;
			}else if(choice == 4){
				m.MESSAGE();
				continue;
			}else if(choice == 5){
				try{
					op_mode = new CTR(cLog, key);
					if(op_mode == nullptr)
						throw std::bad_alloc();
				}catch(std::exception& e){
					std::cerr << "[ERROR]: " << e.what() << std::endl;
					return -1;
				}
			}
			else if(choice == 9){
				m.MESSAGE("Go Back to Main Menu...", 1);
				continue;
			}

			if(isEncryption)
			{
				do_stream(p, [op_mode](const BLOCK& msg) -> BLOCK { return op_mode->encrypt_mode(msg);});
			}
			else
			{
				do_stream(p, [op_mode](const BLOCK& msg) -> BLOCK { return op_mode->decrypt_mode(msg);});
			}

			delete op_mode;

		}else if(choice == 8){
		//Setting
			m.setting(choice);

			if(choice == 1){
			//Setting file path
				while(!setPath(p));
				isSetFile = true;

			}else if(choice == 2){
				if(!isSetFile){
					m.MESSAGE("There is no set file path", -1);
				}else{
					std::cout << "Now Set File Path: " << fs::canonical(p);
				}
				std::cout << std::endl;

			}else if(choice == 3){
			//Diagonsis Crypto Logic
				m.MESSAGE();
			}else if(choice == 4){
			//print out program information			
				m.information();

			}else if(choice != 9){
				//to do not disturb printing main menu, use cout(not cerr)
				m.MESSAGE("Please Select Number On Menu", -1);
			}
		}else if(choice == 9){
		//Terminate Program
			m.MESSAGE("Terminate Program...", 1);
			break;
		}else{
			//to do not disturb printing main menu, use cout(not cerr)
			m.MESSAGE("Please Select Number On Menu", -1);
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

bool do_stream(fs::path p, std::function<BLOCK(const BLOCK&)> run)
{
	std::ofstream fout;
	std::ifstream fin;
	//1024 byte = 1KB
	char buffer[1024];
	BLOCK B_buffer;
	//make temp and rename
	fs::path temp_path = p;
	temp_path += ".tmp";

#ifdef LOG
	log("Start to open File");
#endif
	fout.open(temp_path, std::ios::binary);
	if(fout.fail())
		return false;
	fin.open(p, std::ios::binary);
	if(fin.fail())
	{
		fout.close();
		return false;
	}
#ifdef LOG
	log("Success to open File");
	log("Start to loop");
#endif
	while(true)
	{
#ifdef LOG
		log("Start to read and interpret file");
#endif
		fin.read(buffer, sizeof(buffer));
		std::streamsize size = fin.gcount();

		if(size <= 0)
		{
#ifdef LOG
			if(fin.eof())
				log("File is Normally readch EOF");
			else if(fin.fail())
				log("Stream Failbit occur");
			else if(fin.bad())
				log("Stream Badbit Occur");
			std::cout << "size: " << size << std::endl;
#endif
			break;
		}
		B_buffer.resize(size);
		//interpret to vector<byte>
		std::memcpy(B_buffer.data(), buffer, size);
#ifdef LOG
		log("Success to read and interpret file");
		log("Start to run crypto logic");
#endif
		try{
			//do crypto logic (have exception logic)
			BLOCK result = run(B_buffer);
#ifdef LOG
			log("Success to run crypto logic");
			log("Start to write file");
#endif
			fout.write(reinterpret_cast<const char*>(result.data()), result.size());
		}catch(std::exception& e)
		{
			//close file
			fout.close();
			fin.close();
			//if exception occur, erase temp file
			fs::remove(temp_path);

			std::cerr << "[ERROR]: " << e.what() << std::endl;
			return false;
		}
#ifdef LOG
		log("Success to write file");
#endif
	}
	//close file
	fout.close();
	fin.close();

	fs::rename(temp_path, p);

	return true;
}
