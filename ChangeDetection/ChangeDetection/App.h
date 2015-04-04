#pragma once

#include "IOHandler.h"
#include "MyStringUtility.h"


class App
{
private:
	IOHandler* ioHandler;
	MyStringUtility* strUtilHandle;
	std::string log;

	int dictionarySize;

	std::vector< std::vector <double> > r;
	std::vector< String2intMap > allData;
	std::vector< std::vector <double> > testData;

	//change detection algorithm
	String2intMap getx_t(int t, Method method);
	void initializeChangeDetectionAlgorithm(Method method);
	void runChangeDetectionAlgorithm(Method method);
	double calculateLikelihood (String2intMap data, int dictionarySize);
	double getInitialProbability();
	double hazardFunction(bool isChangePoint);

	//pre-processing
	String2intMap getWordFrequencyCount(std::string inputText);
	String2intMap getPOSCount(int fileNumber);

	//utilities
	std::vector <int> hash2Vector(String2intMap inputMap);
	String2intMap mergeString2intMaps(std::vector< String2intMap > inputList, int startIndex, int endIndex);
	double sumOfElements(std::vector <double> inputVector);
	std::string fileNUmber2FilePath (int fileNumber);

public:
	App(void);
	~App(void);

	void run(Method method);

	void prin2DArray(std::vector< std::vector <double> > inputData);
	void printString2intMap(String2intMap inputMap);
	
	void testTokenizer();
	std::vector< std::vector <double> > makeTestData();
};

