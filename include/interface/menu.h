#ifndef MENU_H
#define MENU_H
#include <string>
#include <string_view>
#include <cstdint>
#include "cryptologic/crypto.h"

class menu{
private:
	void safe_input(int&);
	void take_hex(block);
public:
	struct ANSI
	{
		ANSI() = delete;
		//COLOR 
		static constexpr std::string_view COLOR_RESET	= "\033[0m";
		static constexpr std::string_view BLACK	 	= "\033[30m";
		static constexpr std::string_view RED   	= "\033[31m";
		static constexpr std::string_view GREEN 	= "\033[32m";
		static constexpr std::string_view YELLOW 	= "\033[33m";
		static constexpr std::string_view BLUE 		= "\033[34m";
		static constexpr std::string_view MAGENTA 	= "\033[35m";
		static constexpr std::string_view CYAN 		= "\033[36m";

		//Screen & Cursor
		static constexpr std::string_view SCR_RESET 	= "\033[2J";
		static constexpr std::string_view CUR_HOME	= "\033[H";
	};

	void main(int&);
	void take_key(crypto::crypto_logic, block&);
	void crypto_logic(int&);
	void op_mode(int&);
	void setting(int&);
	void information(void);

	void MESSAGE(const char* = "A", int = 0);
};
#endif
