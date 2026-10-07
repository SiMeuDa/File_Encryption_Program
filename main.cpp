//Base Header
#include "cryptologic/crypto.h" 
//Crypto Logic
#include "cryptologic/DES/DES.h"
#include "cryptologic/DES/Triple_DES.h"
//#incldue "cryptologic/AES/AES_128.h"
//operation modes
#include "cryptologic/mode/mode.h"
#include "cryptologic/mode/CTR.h"
//muti-threading queue
//#include "interface/thread_queue.h"
#include <thread>
#include <future>
#include <chrono>
//STDIO
#include <iostream>
//file IO class
#include <fstream>
#include <filesystem>
#include <cstring>
//data manipulate class(for API input)
#include <cstdint>
#include <array>
//user input
#include <string>
#include "interface/menu.h"
//exception handling
#include <stdexcept>
#include <memory>
#include <functional>


namespace fs = std::filesystem;

bool setPath(fs::path& p);
bool do_stream(fs::path, std::function<void(const block, block&, size_t&, bool)>);

int main(void)
{
	fs::path p;
	menu m;
	int choice = 0;
	uint64_t int_key = 0, int_key2 = 0;
	block key;
	bool isSetFile = false, isEncryption = true;
	mode* op_mode;
	crypto::crypto_logic cLog;
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	std::chrono::milliseconds duration;

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
			m.take_key(cLog, key);
			
			//set operation mode
			m.op_mode(choice);
			try{
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
					op_mode = new CTR(cLog, key);
				}else if(choice == 9){
					m.MESSAGE("Go Back to Main Menu...", 1);
					continue;
				}
			}catch(std::exception& e){
				std::cerr << "[ERROR]: " << e.what() << std::endl;
				return -1;
			}
			start = std::chrono::high_resolution_clock::now();
			if(isEncryption)
			{
				do_stream(p, [op_mode](const block msg, block& output, size_t& size, bool eof) 
				{ op_mode->encrypt_mode(msg, output, size, eof);});
			}
			else
			{
				do_stream(p, [op_mode](const block msg, block& output, size_t& size, bool eof)
				{ op_mode->decrypt_mode(msg, output, size, eof);});
			}

			delete op_mode;
			
			end = std::chrono::high_resolution_clock::now();

			duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

			std::clog << menu::ANSI::BLUE << "[SYSTEM]: Total Taken Time: " << duration.count() << "ms" << menu::ANSI::COLOR_RESET << std::endl;

		}else if(choice == 8)
		{
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

bool do_stream(fs::path p, std::function<void(const block, block&, size_t&, bool)> run)
{
	std::ofstream fout;
	std::ifstream fin;
	size_t buffer_size = 1024 * 256;
	//time check
	std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
	std::chrono::milliseconds duration = std::chrono::milliseconds::zero();
	//1024 * 1024 byte = 1024 KB = 1 MB
	block rd_buffer = new uint8_t[buffer_size];
	block wt_buffer = new(std::nothrow) uint8_t[buffer_size];
	if(wt_buffer == nullptr)
	{
		delete[] rd_buffer;
		return false;
	}

	//make temp and rename
	fs::path temp_path = p;
	temp_path += ".tmp";

	fout.open(temp_path, std::ios::binary);
	if(fout.fail())
		return false;

	fin.open(p, std::ios::binary);
	if(fin.fail())
	{
		fout.close();
		return false;
	}
	while(true)
	{
		fin.read(reinterpret_cast<char*>(rd_buffer), buffer_size);
		//if use size_t, it can cause overflow
		std::streamsize raw_size = fin.gcount();

		if(raw_size <= 0)
			break;
			
		size_t size = static_cast<size_t>(raw_size);

		try{
			start = std::chrono::high_resolution_clock::now();
			//do crypto logic (have exception logic)
			run(rd_buffer, wt_buffer, size, fin.eof());

			end = std::chrono::high_resolution_clock::now();
			
			duration += std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

			fout.write(reinterpret_cast<char*>(wt_buffer), size);
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
	}
	//close file
	fout.close();
	fin.close();

	delete[] wt_buffer;
	delete[] rd_buffer;

	fs::rename(temp_path, p);

	std::clog << menu::ANSI::BLUE << "[SYSTEM]: Crypto Logic Taken Time: " << duration.count()
		<< "ms" << menu::ANSI::COLOR_RESET << std::endl;

	return true;
}
