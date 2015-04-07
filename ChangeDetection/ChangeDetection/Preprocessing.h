#pragma once

#include "Features.h"
#include "MyStringUtility.h"

class Preprocessing
{
private:

	MyStringUtility* strUtilHandle;

	StringList NON_FUNCTION_POS;

	String2intMap globalWordCount;
	String2intMap globalPOSCount;
	String2intMap globalFunctionWordCount;

	//text preprocessings
	void runPOSTagger(int fileNumber, std::string inputFilePath);

	//utilities
	bool isElementInList(std::string element, StringList list);

public:
	Preprocessing(void);
	~Preprocessing(void);
	int getWordCountDictSize();
	int getPOSCountDictSize();
	int getFunctionWordCountDictSize();

	//text preprocessings interfaces
	void runTextPreprocessing(StringList fileNames, std::string filePath);
	Features* getFeatures(std::string text);
};

