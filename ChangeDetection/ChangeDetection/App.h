#pragma once

#include "IOHandler.h"


class App
{
private:
	IOHandler* ioHandler;
	std::string log;

	int dictionarySize;

	std::vector< std::vector <double> > r;
	std::vector< String2intMap > allData;
	std::vector< std::vector <double> > testData;

	//change detection algorithm
	String2intMap getx_t(int t);
	void initializeChangeDetectionAlgorithm();
	void runChangeDetectionAlgorithm();
	double calculateLikelihood (String2intMap data, int dictionarySize);
	double getInitialProbability();
	double hazardFunction(bool isChangePoint);

	//pre-processing
	String2intMap getWordFrequencyCount(std::string inputText);

	//utilities
	std::vector <int> hash2Vector(String2intMap inputMap);
	String2intMap mergeString2intMaps(std::vector< String2intMap > inputList, int startIndex, int endIndex);
	double sumOfElements(std::vector <double> inputVector);

public:
	App(void);
	~App(void);

	void run();

	void prin2DArray(std::vector< std::vector <double> > inputData);
	void printString2intMap(String2intMap inputMap);
	
	void testTokenizer();
	std::vector< std::vector <double> > makeTestData();
};

