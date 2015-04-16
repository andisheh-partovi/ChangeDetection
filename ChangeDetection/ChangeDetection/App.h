#pragma once

#include "Preprocessing.h"
#include "IOHandler.h"
#include "VectorAndMapUtility.h"

enum Method
{
	WORDCOUNT,
	POSCOUNT,
	FUNCTIONWORDCOUNT
};

enum DataSet
{
	TEST,
	STATE_OF_THE_UNION,
	TWEET
};

class App
{
private:

	//handles and inter-program variables
	IOHandler* ioHandler;
	Preprocessing* preprocessHandle;
	VectorAndMapUtility* utlityHandle;
	std::string log;

	//domain specific
	int dictionarySize;
	double hazardRate;
	Method method;
	bool isLogSpace;
	std::set<std::string> dictionary;

	unsigned int totalTimeSteps;
	std::vector<Features*> allFeatues;
	std::vector< std::vector <long double> > r;
	std::vector <long double> max_r;
	std::vector< String2doubleMap > allData;
	std::vector< int > allDataSizes;
	std::vector< std::vector <long double> > testData;

	//change detection algorithm
	void feedData (String2doubleMap x_t);
	String2doubleMap getx_t(int t);
	String2doubleMap normalize_x_t (String2doubleMap raw_x_t);
	void initializeChangeDetectionAlgorithm();
	void runChangeDetectionAlgorithm();
	void runLogChangeDetectionAlgorithm();
	long double calculateLikelihood (String2doubleMap data, int dictionarySize);
	long double calculateLogLikelihood (String2doubleMap data, int dictionarySize);
	long double getInitialProbability();
	long double hazardFunction(bool isChangePoint);

	//utilities
	double getAverageLengthInRange(std::vector< int > allDataSizes);
	std::vector <int> getMaxProbabilityindexAtEachTime(std::vector< std::vector <long double> > r);

public:
	App(void);
	~App(void);

	void run(Method method, DataSet dataSet, bool doParse, bool isLogSpace);

	void print1DArray(std::vector <int> inputData);
	void prin2DArray(std::vector< std::vector <long double> > inputData);
	void printString2intMap(String2doubleMap inputMap);
	
	void testTokenizer();
	std::vector< std::vector <long double> > makeTestData();
};

