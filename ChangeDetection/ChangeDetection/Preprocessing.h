#pragma once

#include "Features.h"
#include "MyStringUtility.h"

class Preprocessing
{
private:

	MyStringUtility* strUtilHandle;

	StringList NON_FUNCTION_POS;
	StringList stopWords;

	String2doubleMap globalWordCount;
	String2doubleMap globalPOSCount;
	String2doubleMap globalFunctionWordCount;

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
	void setStopWordsList(std::string inputFileContent);
	StringList getStopWords();

	//text preprocessings interfaces
	void runTextPreprocessing(StringList fileNames, std::string filePath);
	Features* getFeatures(std::string text);
};

