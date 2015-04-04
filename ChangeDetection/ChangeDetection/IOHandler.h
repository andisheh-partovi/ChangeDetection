#pragma once

#include "Definitions.h"
#include <fstream>

class IOHandler
{
public:
	IOHandler(void);
	~IOHandler(void);

	std::string readFile(std::string filePath);
	void write2File(std::string text, std::string filePath);
};

