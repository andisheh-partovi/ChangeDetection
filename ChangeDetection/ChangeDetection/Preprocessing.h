#pragma once

#include "IOHandler.h"

class Preprocessing
{
private:
	IOHandler* ioHandler;

	//text preprocessings
	void runPOSTagger(int fileNumber, std::string inputFilePath);

public:
	Preprocessing(void);
	~Preprocessing(void);

	//text preprocessings interfaces
	void runTextPreprocessing(StringList fileNames, std::string filePath);

};

