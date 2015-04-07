#pragma once

#include "Definitions.h"

#include <dirent.h>
#include <fstream>

class IOHandler
{
public:
	IOHandler(void);
	~IOHandler(void);

	//generic
	std::string readFile(std::string filePath);
	void write2File(std::string text, std::string filePath);
	StringList getAllFilesIndirectory(std::string directoryPath);
	
	//project specific
	std::string fileNUmber2FilePath (int fileNumber);
	std::string getPOSTags(int fileNumber);
};

