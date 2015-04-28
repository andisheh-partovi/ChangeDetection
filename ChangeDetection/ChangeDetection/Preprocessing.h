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
	String2doubleMap globalStopWordCount;

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
	int getStopWordCountDictSize();
	void setStopWordsList(std::string inputFileContent);
	StringList getStopWords();
	String2doubleMap getWordCountDict();
	String2doubleMap getFunctionWordCountDict();
	String2doubleMap getStopWordCountDict();

	//text preprocessings interfaces
	void runTextPreprocessing(StringList fileNames, std::string filePath);
	Features* getAllFeatures(std::string text);
	Features* getWordCountFeature(std::string text);
	Features* getFunctionWordCountFeature(std::string text);
	Features* getStopWordCountFeature(std::string text);
};

