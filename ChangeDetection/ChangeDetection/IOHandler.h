#pragma once

#include "Definitions.h"
#include <fstream>

class IOHandler
{
public:
	IOHandler(void);
	~IOHandler(void);

	std::string readFile(std::string filePath);
};

