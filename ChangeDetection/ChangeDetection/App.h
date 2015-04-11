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
	std::vector< String2doubleMap > allData;
	std::vector< int > allDataSizes;
	std::vector< std::vector <long double> > testData;

	//change detection algorithm
	void feedData (String2doubleMap x_t);
	String2doubleMap getx_t(int t, Method method);
	String2doubleMap normalize_x_t (String2doubleMap raw_x_t);
	void initializeChangeDetectionAlgorithm(Method method);
	void runChangeDetectionAlgorithm(Method method);
	long double calculateLikelihood (String2doubleMap data, int dictionarySize);
	long double getInitialProbability();
	long double hazardFunction(bool isChangePoint);

	//utilities
	std::vector <int> hash2Vector(String2doubleMap inputMap);
	String2doubleMap mergeString2intMaps(std::vector< String2doubleMap > inputList, int startIndex, int endIndex);
	long double sumOfElements(std::vector <long double> inputVector);
	int sumOfElements(String2doubleMap inputMap);
	double getAverageLengthInRange(std::vector< int > allDataSizes);

public:
	App(void);
	~App(void);

	void run(Method method);

	void prin2DArray(std::vector< std::vector <long double> > inputData);
	void printString2intMap(String2doubleMap inputMap);
	
	void testTokenizer();
	std::vector< std::vector <long double> > makeTestData();
};

