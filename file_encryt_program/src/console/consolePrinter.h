#pragma once
#include "iprogress.h"
#include <string>

class ConsolePrinter : public IProgress {
private:
	bool m_showText;
	const int BAR_WIDTH;

public:
	const std::string RED;
	const std::string BLUE;
	const std::string BOLD;
	const std::string YELLOW;
	const std::string RESET;

	ConsolePrinter();
	void update(double fraction) override;
};
