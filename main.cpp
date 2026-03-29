/*
   		FILE EN/DECRYPTION PROGRAM
		USED CIPHER LOGIC: Triple DES
 */
#include "include/cryptologic/cipher.h"
#include "include/interface/consolePrinter.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <unistd.h> 
#include <cctype>
using namespace std;

const char VERSION[6] = "0.1.1";

//Checking Function
inline bool isValidPath(const std::string&);
//Mutual Function
inline void chckArgu(int, char**);
inline void takeKey(int, char**, uint64_t&, uint64_t&);
//Divided to En/De Function
inline void readValue(const char*, std::string&);
inline void readValue(const char*, std::vector<uint64_t>&);
inline void writeValue(const char*, const std::vector<uint64_t>&);
inline void writeValue(const char*, const std::string&);

int main(int argc, char* argv[])
{
	ConsolePrinter printer;
	cipher c;
	uint64_t key, key2;
	string str;

	try{
		//check Argument
		//If Invalid Argument, throw exception
		chckArgu(argc, argv);

		//set loading console
		c.setProgressCallback(&printer);

		//take key
		//If OverFlow or Invalid Value, throw exception
		takeKey(argc, argv, key, key2);
		
		//check cryption argument
		if(argv[2] == "1"sv)
		{//Encryption
			
			//read value from file
			readValue(argv[4], str);
	
			//encrypt logic
			std::vector<uint64_t> vecResult = c.encryption(str, key, key2);
			if(vecResult.empty())
				throw std::length_error("[Length Error]: Failed to Encrypt File");

			//write value to file
			writeValue(argv[4], vecResult);

			clog << printer.BOLD << printer.BLUE << "[SYSTEM]: SUCCESS TO ENCRYPT FILE" << printer.RESET << endl;
		}
		else if(argv[2] == "0"sv)
		{//Decryption
			
			std::vector<uint64_t> vecInput;

			//read value from file
			readValue(argv[4], vecInput);

			//decrypt logic
			str = c.decryption(vecInput, key, key2);
			if(str.empty())
				throw std::length_error("[Length Error]: Failed to Decrypt File");

			//write value to file
			writeValue(argv[4], str);

			clog << printer.BOLD << printer.BLUE << "[SYSTEM]: SUCCESS TO DECRYPT FILE" << printer.RESET << endl;
		}
		else	//Invalid Data Type
				throw std::invalid_argument("[Invalid Input]: -en argument must be 1 or 0");
	}catch(const std::exception& e)
	{
		cerr << printer.BOLD << printer.RED << "[Error]: " << e.what() << printer.RESET << endl;
		return -1;
	}

	return 0;
}

inline bool isValidPath(const std::string& path)
{//check whether file path is valid
    return access(path.c_str(), F_OK) == 0;
}

inline void chckArgu(int argc, char** argv)
{
	//isEncryption: 1, filePath: 1, key: 1
	if(((argc == 7) || (argc == 8)) && (argv[3] == "-f"sv) && (argv[5] == "-k"sv) && (argv[1] == "-en"sv))
	{//Standard DES section
	//foam: ./[file_name] -en [1/0] -f [file_path] -k [key]
		if(!isValidPath(argv[4]))
			throw std::invalid_argument("[Invalid Input]: Wrong File Path");
	}
	else	//Invalid parameter input
		throw invalid_argument("Invalid Input\n\033[0m\033[1m[Usage]: ./[file_name] -en [1/0] -f [file_path] -k [key] ([key2])");
}

inline void takeKey(int argc, char** argv, uint64_t& key, uint64_t& key2)
{
		//UINT64_MAX = 18,446,744,073,709,551,615 = 0xFFFFFFFFFFFFFFFF
		try{
			key = stoull(argv[6]);

			if(argc == 8)
				key2 = stoull(argv[7]);
			else if(argc == 7)
				key2 = key;

		}catch(const out_of_range& e)
		{
			key = 0;
			key2 = 0;
			throw std::overflow_error("[Runtime Error]: Key Value is too Big");
		}
		catch(const invalid_argument& e)
		{
			key = 0;
			key2 = 0;
			throw invalid_argument("[Invalid Input]: Key Value must be ULL Type");
		}
}

//ENCRYPTION LOGIC
inline void readValue(const char* path, std::string& str)
{
	//read from file
	fstream file;

	//open file for binary -> reduce program running time
	file.open(path, ios::in | ios::binary | ios::ate);		
	if(file.fail())
		throw std::invalid_argument("[Invalid Input]: Failed to Open File");
	
	//take file msg
	std::streamsize size = file.tellg();
	file.seekg(0, ios::beg);

	str.resize(size);

	if(size > 0)
		file.read(str.data(), size);

	file.close();
}

inline void readValue(const char* path, std::vector<uint64_t>& vec)
{
	//read from file
	fstream file;

	//open for Input, for binary, At end
	file.open(path, ios::in | ios::binary | ios::ate);
	if(file.fail())
		throw std::invalid_argument("[Invalid Input]: Failed to Open File");

	//check file size
	streamsize size = file.tellg();
	
	if(size % sizeof(uint64_t) != 0)
		throw std::invalid_argument("[Invalid Input]: Invalid File (size no aligned)");

	//return to begin
	file.seekg(0, ios::beg);
	
	//for resize vector
	size_t count = size / sizeof(uint64_t);
	
	vec.resize(count);

	if(count <= 0)
		throw length_error("[Length Error]: Failed to Read File");
	
	//take value once to whole vector
	file.read(reinterpret_cast<char*>(vec.data()), size);
	
	file.close();
}

//ENCRYPTION LOGIC
inline void writeValue(const char* path, const std::vector<uint64_t>& vec)
{
	//write to file
	fstream file;
	
	//open file
	file.open(path, ios::out | ios::trunc | ios::binary);
	if(file.fail())
		throw invalid_argument("[Invalid Input]: Failed to Open File");
	
	//write whole vector
	file.write(reinterpret_cast<const char*>(vec.data()), vec.size() * sizeof(uint64_t));
	file.close();
}

//DECRYPTION LOGIC
inline void writeValue(const char* path, const std::string& str)
{
	//write to file
	fstream file;

	//open file
	file.open(path, ios::out | ios::binary | ios::trunc);
	if(file.fail())
		throw invalid_argument("[Invalid Input]: Failed to Open File");
	
	//write string
	file.write(str.data(), str.length());
	file.close();
}
