#pragma once

#include "Preprocessing.h"
#include "IOHandler.h"

enum Method
{
	wordCount,
	POSCount,
	functionWordCount
};

class App
{
private:

	//handles and inter-program variables
	IOHandler* ioHandler;
	Preprocessing* preprocessHandle;
	std::string log;

	//domain specific
	int dictionarySize;

	unsigned int totalTimeSteps;
	std::vector<Features*> allFeatues;
	std::vector< std::vector <long double> > r;
	std::vector< String2intMap > allData;
	std::vector< std::vector <long double> > testData;

	//change detection algorithm
	String2intMap getx_t(int t, Method method);
	void initializeChangeDetectionAlgorithm(Method method);
	void runChangeDetectionAlgorithm(Method method);
	long double calculateLikelihood (String2intMap data, int dictionarySize);
	long double getInitialProbability();
	long double hazardFunction(bool isChangePoint);

	//utilities
	std::vector <int> hash2Vector(String2intMap inputMap);
	String2intMap mergeString2intMaps(std::vector< String2intMap > inputList, int startIndex, int endIndex);
	long double sumOfElements(std::vector <long double> inputVector);

public:
	App(void);
	~App(void);

	void run(Method method);

	void prin2DArray(std::vector< std::vector <long double> > inputData);
	void printString2intMap(String2intMap inputMap);
	
	void testTokenizer();
	std::vector< std::vector <long double> > makeTestData();
};

