#pragma once

#include "Preprocessing.h"
#include "IOHandler.h"
#include "VectorAndMapUtility.h"
#include "SufficientStatistics.h"

enum Method
{
	WORDCOUNT,
	STOPWORDCOUNT,
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
	std::string logString;

	//domain specific
	int dictionarySize;
	double hazardRate;
	double logHazardRate;
	Method method;
	bool isLogSpace;
	std::set<std::string> dictionary;

	//algorithm variables
	unsigned int totalTimeSteps;
	std::vector<Features*> allFeatues;
	std::vector< std::vector <long double> > r;
	std::vector< std::vector <SufficientStatistics*> > sufStats;
	std::vector <long double> max_r;
	std::vector< String2doubleMap > allData;
	std::vector< int > allDataSizes;
	std::vector< std::vector <long double> > testData;
	String2doubleMap allDictionary;

	//change detection algorithm functions:
	//admin
	void runAlgorithm1();
	void runAlgorithm2();
	//common
	void feedData (String2doubleMap x_t);
	String2doubleMap getx_t(int t);
	String2doubleMap normalize_x_t (String2doubleMap raw_x_t);
	void initializeChangeDetectionAlgorithm();
	long double getInitialProbability();
	Features* getFeature(int timeStep);
	//normal
	void runChangeDetectionAlgorithm();
	long double calculateLikelihood (String2doubleMap data, int dictionarySize);
	long double hazardFunction(bool isChangePoint);
	//log space
	void runLogChangeDetectionAlgorithm();
	long double calculateLogLikelihood (String2doubleMap data, int dictionarySize);
	long double logHazardFunction(bool isChangePoint);
	//new algorithm
	void runLogChangeDetectionAlgorithm2();
	String2doubleMap calculateU_D(String2doubleMap data, String2doubleMap dictionary);
	long double calculateLogLikelihood2 (String2doubleMap data, int alpha, String2doubleMap beta);
	String2doubleMap uniformDistribution(String2doubleMap data);
	SufficientStatistics* updateSuffiecientStats(SufficientStatistics* previousSufStats, String2doubleMap U);
	SufficientStatistics* initialiseSufStats();

	//utilities
	double getAverageLengthInRange(std::vector< int > allDataSizes);
	std::vector <int> getMaxProbabilityindexAtEachTime(std::vector< std::vector <long double> > r);
	long double logSumExp(std::vector <long double> logVector);

public:
	App(void);
	~App(void);

	void run(Method method, DataSet dataSet, bool doParse, bool isLogSpace, int algorithmNumber);
	
	void testTokenizer();
	std::vector< std::vector <long double> > makeTestData();
};

