#include "App.h"

#include <boost\tokenizer.hpp>
#include <boost/lexical_cast.hpp>//casting long double to string
#include <boost/math/special_functions/gamma.hpp> //gamma function (tgamma)
#include <math.h> //pow


App::App(void)
{
	ioHandler = new IOHandler();
	preprocessHandle = new Preprocessing();
	utlityHandle = new VectorAndMapUtility();
}

void App::run(Method method, DataSet dataSet, bool doParse, bool isLogSpace, int algorithmNumber)
{
	std::cout << "started running\n\n";

	//------------------------------------------ 1.Preprocessing
	std::string dataFilesPath = PROJECT_PATH;
	switch(dataSet)
	{
	case TEST:
		dataFilesPath += "\\test_files\\";
		hazardRate = 0.1666666; //used in the prioir probability over change P(r_t | r_t-1) for synthetic data's test : 1/6
		break;
	case STATE_OF_THE_UNION:
		dataFilesPath += "\\state_union\\";
		hazardRate = 0.25;// 1/4 //0.1176; // 2/17
		break;
	default:
		std::cerr << "ERROR: unknown dataset selected.";
	}

	logHazardRate = log (hazardRate);
	
	//feeding all the data files to the pre-process unit to get them processed
	StringList allDataFileNames = ioHandler->getAllFilesIndirectory(dataFilesPath);

	if (doParse)
		preprocessHandle->runTextPreprocessing( allDataFileNames , dataFilesPath);

	totalTimeSteps = allDataFileNames.size();

	//feed the list of stop words to the pre-processing unit
	preprocessHandle->setStopWordsList(ioHandler->readFile("stopWordsList.txt"));
	//ioHandler->print1DArray(preprocessHandle->getStopWords());//testing

	this->method = method;
	this->isLogSpace = isLogSpace;

	//run the algorithm:
	if (algorithmNumber == 1)
		runAlgorithm1();
	else if (algorithmNumber == 2)
		runAlgorithm2();
	else
		std::cerr << "ERROR: wrong algorithm number.\n";

	std::cout << "finished running\n\n";

	//outputting:
	//writing the log:
	ioHandler->write2File(logString, "log.txt");
	ioHandler->print2DArray(r);

	//also print the maximums, which is what we actuallt want
	std::vector <int> maxes = getMaxProbabilityindexAtEachTime(r);
	ioHandler->print1DArray(maxes);

	//std::cout << "\n\n";
	//ioHandler->printMap(utlityHandle->mergeMaps(allData, 0, allData.size()-1));
}

void App::runAlgorithm1()
{
	initializeChangeDetectionAlgorithm();

	////------------------------------------- 2.Running the algorithm
	if (isLogSpace)
		runLogChangeDetectionAlgorithm();
	else
		runChangeDetectionAlgorithm();
}

void App::feedData(String2doubleMap x_t)
{
	allDataSizes.push_back(utlityHandle->sumOfElements(x_t));
	allData.push_back(x_t);
	std::set<std::string> currentDictionary = utlityHandle->getUniqueKeys(x_t);
	dictionary.insert( currentDictionary.begin(), currentDictionary.end() );
	//ioHandler->printMap(allData[allData.size() - 1]); //testing
}

Features* App::getFeature(int timeStep)
{
	switch (this->method)
	{
		case WORDCOUNT:
			return preprocessHandle->getWordCountFeature(ioHandler->getPOSTags(timeStep));
		case STOPWORDCOUNT:
			return preprocessHandle->getStopWordCountFeature(ioHandler->getPOSTags(timeStep));
		case FUNCTIONWORDCOUNT:
			return preprocessHandle->getFunctionWordCountFeature(ioHandler->getPOSTags(timeStep));
		default:
			std::cerr << "ERROR: unkown method selected:" << boost::lexical_cast<std::string>(method);
	}
}

//the initialization steps
void App::initializeChangeDetectionAlgorithm()
{
	allFeatues.push_back(getFeature(0));

	//setting the first element of the r matrix to its initial value
	std::vector<long double> tempRow;
	tempRow.push_back(getInitialProbability());
	r.push_back(tempRow);
	
	//reading and saving the first datum
	feedData(getx_t(0));
}

void App::runChangeDetectionAlgorithm()
{
	long double likelihood;
	long double P_rt_and_x_1_t;
	std::vector<long double> joint_rt_probs;
	long double evidence;
	String2doubleMap x_t;

	//While there is a new datum available
	//for all files in the data folder
	for (unsigned int t = 1 ; t < /*30*/ totalTimeSteps ; ++t)
	{
		std::cout << "\ntimestep: " << t << "\n--------------------------------\n\n";
		logString += "timestep: " + std::to_string(static_cast<long long>(t)) + ":\n";

		//Preprocess the datum to get the xt
		//the process can happen here or already happend in another module
		feedData(getx_t(t));

		//for all the points in r_t, calculate the joint probability
		for (unsigned int i = 0 ; i < t+1 ; ++i)
		{
			if ( i == 0) //possible changepoint : calcuate changepoint probability
			{
				P_rt_and_x_1_t = 0;

				//ioHandler->printMap(normalize_x_t(allData[t])); std::cout<<"\n";//testing
				likelihood = calculateLikelihood(normalize_x_t(allData[t]), dictionary.size());

				//for all the points in r_t-1
				for (unsigned int j = 0 ; j < r.at(t-1).size() ; ++j)
				{
					P_rt_and_x_1_t += r[t-1][j] * likelihood * hazardFunction(true);
				}
			}
			else //possible continuation : calcuate Growth probability
			{
				if (r[t-1][i-1] != 0)
				{
					//ioHandler->printMap(normalize_x_t(utlityHandle->mergeMaps(allData, t-i, t))); std::cout<<"\n";//testing
					likelihood = calculateLikelihood(normalize_x_t(utlityHandle->mergeMaps(allData, t-i, t)), dictionary.size());
					P_rt_and_x_1_t = r[t-1][i-1] * likelihood * hazardFunction(false);
				}
				else
				{
					P_rt_and_x_1_t = 0;
				}
			}

			//save the joint probabilities
			joint_rt_probs.push_back(P_rt_and_x_1_t);
			
			//log:
			logString += "for point r_t = " + std::to_string(static_cast<long long>(i))
				+ ":\nlikelihood: " + boost::lexical_cast<std::string>(likelihood) + "\n"
				+ ":\njoint: " + boost::lexical_cast<std::string>(P_rt_and_x_1_t) + "\n";
		}

		//for all the points in r_t, calculate the conditional probability
		//first get the sum (evidence probability)
		evidence = utlityHandle->sumOfElements(joint_rt_probs);

		//then divide all the joints by the sum to get the conditional prob
		for (unsigned int i = 0 ; i < t+1 ; ++i)
		{
			joint_rt_probs.at(i) = joint_rt_probs.at(i) / evidence;

			////remember: joiny_rt_probs is now holding conditionals not joints
			////intorcusing thresholds:
			//if (joint_rt_probs.at(i) < SIGNIFICANT_PROBABILITY_THRESHOLD)
			//	joint_rt_probs.at(i) = 0;
		}

		//remember: joiny_rt_probs is now holding conditionals not joints
		r.push_back(joint_rt_probs);
		joint_rt_probs.clear();
	}
}

void App::runLogChangeDetectionAlgorithm()
{
	long double logLikelihood;
	long double logP_rt_and_x_1_t;
	std::vector<long double> joint_rt_probs;
	std::vector<long double> a_i;
	long double evidence;
	String2doubleMap x_t;

	//While there is a new datum available
	//for all files in the data folder
	for (unsigned int t = 1 ; t < /*17*/totalTimeSteps ; ++t)
	{
		std::cout << "\ntimestep: " << t << "\n--------------------------------\n";
		logString += "\ntimestep " + std::to_string(static_cast<long long>(t)) + ":\n\n\n";
		
		//testing:
		//std::cout << "\ndictionarySize in this timestep: " << dictionary.size();
		//Preprocess the datum to get the xt
		allFeatues.push_back(getFeature(t));

		//the process can happen here or already happend in another module
		feedData(getx_t(t));

		//for all the points in r_t, calculate the joint probability
		for (unsigned int i = 0 ; i < t+1 ; ++i)
		{
			if ( i == 0) //possible changepoint : calcuate changepoint probability
			{
				//ioHandler->printMap(normalize_x_t(allData[t])); std::cout<<"\n";//testing
				logLikelihood = calculateLogLikelihood(normalize_x_t(allData[t]), dictionary.size());

				//for all the points in r_t-1
				for (unsigned int j = 0 ; j < r.at(t-1).size() ; ++j)
					a_i.push_back( r[t-1][j] + logLikelihood + logHazardFunction(true) );
			}
			else //possible continuation : calcuate Growth probability
			{
				ioHandler->printMap(normalize_x_t(utlityHandle->mergeMaps(allData, t-i, t))); std::cout<<"\n";//testing
				logLikelihood = calculateLogLikelihood(normalize_x_t(utlityHandle->mergeMaps(allData, t-i, t)), dictionary.size());
				a_i.push_back( r[t-1][i-1] + logLikelihood + logHazardFunction(false));
			}

			//log-sum-exp trick:
			logP_rt_and_x_1_t = logSumExp(a_i);

			//save the joint probabilities
			joint_rt_probs.push_back(logP_rt_and_x_1_t);
			a_i.clear();
			
			//log:
			logString += "\nfor point r_t = " + std::to_string(static_cast<long long>(i))
				+ ":\nlikelihood: " + boost::lexical_cast<std::string>(logLikelihood)
				+ "\njoint: " + boost::lexical_cast<std::string>(logP_rt_and_x_1_t) + "\n";

		}//end of rt

		//for all the points in r_t, calculate the conditional probability
		//first get the sum (evidence probability)
		evidence = logSumExp(joint_rt_probs);
		logString += "Normalization factor: " + boost::lexical_cast<std::string>(evidence) + "\n";

		//Normalization:
		//then divide all the joints by the sum to get the conditional prob
		for (unsigned int i = 0 ; i < t+1 ; ++i)
		{
			joint_rt_probs.at(i) = exp (joint_rt_probs.at(i) - evidence);

			////remember: joiny_rt_probs is now holding conditionals not joints
			////intorcusing thresholds:
			//if (joint_rt_probs.at(i) < SIGNIFICANT_PROBABILITY_THRESHOLD)
			//	joint_rt_probs.at(i) = 0;
		}

		//remember: joiny_rt_probs is now holding conditionals not joints
		r.push_back(joint_rt_probs);
		joint_rt_probs.clear();

	}// for all timesteps
}

void App::runLogChangeDetectionAlgorithm2()
{
	long double logLikelihood;
	long double logP_rt_and_x_1_t;
	std::vector<long double> joint_rt_probs;
	std::vector<long double> a_i;
	String2doubleMap U_D;
	long double evidence;
	String2doubleMap x_t;
	int alpha;
	String2doubleMap beta;
	SufficientStatistics* currentsufstat;
	std::vector<SufficientStatistics*> sufstats_t;

	//While there is a new datum available
	//for all files in the data folder
	for (unsigned int t = 1 ; t < totalTimeSteps ; ++t)
	{
		std::cout << "\ntimestep: " << t << "\n--------------------------------\n";
		logString += "\ntimestep " + std::to_string(static_cast<long long>(t)) + ":\n\n\n";

		//the process can happen here or already happend in another module
		allData.push_back(getx_t(t));

		//get U(D_t) for this datum
		U_D = calculateU_D(allData[t], allDictionary);
		//ioHandler->printMap(U_D);

		//std::cout << "\nliklihood: " << calculateLogLikelihood2 (allData[t], 2, beta);

		//for all the points in r_t, calculate the joint probability
		for (unsigned int i = 0 ; i < t+1 ; ++i)
		{
			if ( i == 0) //possible changepoint : calcuate changepoint probability
			{
				//ioHandler->printMap(allData[t]); std::cout<<"\n";//testing
				currentsufstat = initialiseSufStats();
				logLikelihood = calculateLogLikelihood2 (allData[t], currentsufstat->alpha, currentsufstat->beta);

				//for all the points in r_t-1
				for (unsigned int j = 0 ; j < r.at(t-1).size() ; ++j)
					a_i.push_back( r[t-1][j] + logLikelihood + logHazardFunction(true) );
			}
			else //possible continuation : calcuate Growth probability
			{
				//update sufficient statistics:
				currentsufstat = updateSuffiecientStats(sufStats[t-1][i-1], U_D);
				logLikelihood = calculateLogLikelihood2 (allData[t], currentsufstat->alpha, currentsufstat->beta);
				a_i.push_back( r[t-1][i-1] + logLikelihood + logHazardFunction(false));
			}

			//log-sum-exp trick:
			logP_rt_and_x_1_t = logSumExp(a_i);

			//save the joint probabilities
			joint_rt_probs.push_back(logP_rt_and_x_1_t);
			a_i.clear();
			
			//log:
			logString += "\nfor point r_t = " + std::to_string(static_cast<long long>(i))
				+ ":\nlikelihood: " + boost::lexical_cast<std::string>(logLikelihood)
				+ "\njoint: " + boost::lexical_cast<std::string>(logP_rt_and_x_1_t) + "\n";

			//save the current sufstats
			sufstats_t.push_back(currentsufstat);

		}//end of rt

		//for all the points in r_t, calculate the conditional probability
		//first get the sum (evidence probability)
		evidence = logSumExp(joint_rt_probs);
		logString += "Normalization factor: " + boost::lexical_cast<std::string>(evidence) + "\n";

		//Normalization:
		//then divide all the joints by the sum to get the conditional prob
		for (unsigned int i = 0 ; i < t+1 ; ++i)
		{
			joint_rt_probs.at(i) = exp (joint_rt_probs.at(i) - evidence);

			////remember: joiny_rt_probs is now holding conditionals not joints
			////intorcusing thresholds:
			//if (joint_rt_probs.at(i) < SIGNIFICANT_PROBABILITY_THRESHOLD)
			//	joint_rt_probs.at(i) = 0;
		}

		//remember: joiny_rt_probs is now holding conditionals not joints
		r.push_back(joint_rt_probs);
		joint_rt_probs.clear();

		sufStats.push_back(sufstats_t);
		sufstats_t.clear();

	}// for all timesteps
}

SufficientStatistics* App::updateSuffiecientStats(SufficientStatistics* previousSufStats, String2doubleMap U)
{
	int prevAlpha = previousSufStats->alpha;
	int alpha = prevAlpha + 1;
	String2doubleMap beta;
	String2doubleMap prevBeta = previousSufStats->beta;
	std::string word;

	String2doubleMap::iterator iter;
	for (iter = prevBeta.begin() ; iter != prevBeta.end() ; ++iter)
	{
		word = iter->first;
		beta[word] = (prevAlpha * prevBeta[word] / alpha) + (U[word] / alpha);
	}

	return new SufficientStatistics(alpha, beta);
}

String2doubleMap App::uniformDistribution(String2doubleMap data)
{
	String2doubleMap returnMap;
	double value = 1.0 / data.size();

	String2doubleMap::iterator iter;
	for (iter = data.begin() ; iter != data.end() ; ++iter)
	{
		returnMap[iter->first] = value;
	}

	return returnMap;
}

void App::runAlgorithm2()
{
	//preprocessing all data
	for (unsigned int t = 0 ; t < totalTimeSteps ; ++t)
		allFeatues.push_back(getFeature(t));

	//setting the first element of the r matrix to its initial value
	std::vector<long double> tempRow;
	tempRow.push_back(getInitialProbability());
	r.push_back(tempRow);

	//reading and saving the first datum
	allData.push_back(getx_t(0));

	//Setting dictionary size (cheating)
	switch (this->method)
	{
		case WORDCOUNT:
			dictionarySize = preprocessHandle->getWordCountDictSize();
			allDictionary = preprocessHandle->getWordCountDict();
			break;
		case STOPWORDCOUNT:
			dictionarySize = preprocessHandle->getStopWordCountDictSize();
			allDictionary = preprocessHandle->getStopWordCountDict();
			break;
		case FUNCTIONWORDCOUNT:
			dictionarySize = preprocessHandle->getFunctionWordCountDictSize();
			allDictionary = preprocessHandle->getFunctionWordCountDict();
			break;
		default:
			std::cerr << "ERROR: unkown method selected:" << boost::lexical_cast<std::string>(method);
	}

	//testing:
	//std::cout << "\ndictionarySize: " << dictionarySize << "\n";
	//ioHandler->printMap(allDictionary);

	//initialising sufficient statistics
	std::vector<SufficientStatistics*> tempRowStats;
	tempRowStats.push_back(initialiseSufStats());
	sufStats.push_back(tempRowStats);

	runLogChangeDetectionAlgorithm2();
}

SufficientStatistics* App::initialiseSufStats()
{
	int alpha0 = 1;
	String2doubleMap beta0 = uniformDistribution(allDictionary);

	return new SufficientStatistics(alpha0, beta0);
}

long double App::logSumExp(std::vector <long double> logVector)
{
	long double max_a_i = utlityHandle->getMaxElement(logVector);
	long double currentSum = 0;

	for (unsigned int k = 0 ; k < logVector.size() ; ++k)
		currentSum += exp (logVector[k] - max_a_i);

	return max_a_i + log(currentSum);
}

//returns the datum at time t (x_t) in form of a vector
String2doubleMap App::getx_t (int t)
{
	switch (this->method)
	{
		case WORDCOUNT:
			return allFeatues.at(t)->getWordCount();
		case STOPWORDCOUNT:
			return allFeatues.at(t)->getStopWordCount();
		case FUNCTIONWORDCOUNT:
			return allFeatues.at(t)->getFunctionWordCount();
		default:
			std::cerr << "ERROR: unkown method selected:" << boost::lexical_cast<std::string>(method);
	}
}

//normalizes the size of the input data x_t
String2doubleMap App::normalize_x_t (String2doubleMap raw_x_t)
{
	String2doubleMap returnMap;
	double runningAverage = getAverageLengthInRange(this->allDataSizes);
	//std::cout << "\nrunningAverage" << runningAverage << "\n";
	double normalizationFactor = runningAverage / utlityHandle->sumOfElements(raw_x_t);
	//std::cout << "\nnormalizationFactor" << normalizationFactor << "\n";

	String2doubleMap::iterator iter;

	for (iter = raw_x_t.begin() ; iter != raw_x_t.end() ; ++iter)
	{
		returnMap[iter->first] = iter->second * normalizationFactor;
	}

	return returnMap;
}

double App::getAverageLengthInRange(std::vector< int > allDataSizes)
{
	double sum = 0.0;

	for (unsigned int i = 0 ; i < allDataSizes.size() ; ++i)
		sum += allDataSizes[i];

	return sum / allDataSizes.size();
}

String2doubleMap App::calculateU_D(String2doubleMap data, String2doubleMap dictionary)
{
	double sum = utlityHandle->sumOfElements(data);
	String2doubleMap returnMap = dictionary;
	std::string word;

	String2doubleMap::iterator iter;

	for (iter = dictionary.begin() ; iter != dictionary.end() ; ++iter)
	{
		word = iter->first;

		if (data.find(word) == data.end())
			returnMap[word] = 0;
		else
			returnMap[word] = data[word] / sum;
	}

	return returnMap;
}


//original likelihood method
//long double App::calculateLikelihood (String2doubleMap x_t, int dictionarySize)
//{
//	int D = x_t.size();
//	long double alpha = 1.0/dictionarySize;
//	long double gammaAlpha = boost::math::tgamma<long double>(alpha);
//
//	long double constantFactor = boost::math::tgamma<long double>(alpha * D) / pow(gammaAlpha, D);
//
//	long double numerator = 1;
//	long double denom = 0;
//
//	String2doubleMap::iterator iter;
//	for (iter = x_t.begin() ; iter != x_t.end() ; ++iter)
//	{
//		numerator *= boost::math::tgamma<long double>(alpha + iter->second);
//		denom += alpha + iter->second;
//	}
//
//	return (constantFactor * numerator) / boost::math::tgamma<long double>(denom);
//}

//calculates the normal likelihood
long double App::calculateLikelihood (String2doubleMap data, int dictionarySize)
{
	long double alpha = 1.0/dictionarySize;
	long double gammaAlpha = boost::math::tgamma<long double>(alpha);

	long double constantFactor = boost::math::tgamma<long double>(1) / pow(gammaAlpha, dictionarySize);

	long double numerator = 1;
	long double denom = 0;

	String2doubleMap::iterator iter;
	for (iter = data.begin() ; iter != data.end() ; ++iter)
	{
		//if ((alpha + iter->second) > 100)
			//std::cout << "\n\nword: X" << iter->first << "X\n\n";
		//else
		//{
			try{
			numerator *= boost::math::tgamma<long double>(alpha + iter->second);
			} catch (std::exception e){
				std::cout << e.what() << std::endl << "argument:" << (alpha + iter->second);
			}
			denom += alpha + iter->second;
		//}
	}

	for(unsigned int i = 1 ; i <= (dictionarySize - data.size()) ; ++i)
	{
		numerator *= gammaAlpha;
		denom += alpha;
	}

	std::cout << "\n\ndenom: " << denom;

	return (constantFactor * numerator) / boost::math::tgamma<long double>(denom);
}

//calculates the log-likelihood based on equation
long double App::calculateLogLikelihood (String2doubleMap data, int dictionarySize)
{
	long double alpha = 1.0/dictionarySize;
	long double lgammaAlpha = boost::math::lgamma<long double>(alpha);

	long double term1 = dictionarySize * lgammaAlpha;
	long double term2 = 0;
	long double term3 = 1;//it already has sigma(alpha_i)
	long double n;

	String2doubleMap::iterator iter;
	for (iter = data.begin() ; iter != data.end() ; ++iter)
	{
		n = iter->second;

		try{
			term2 += boost::math::lgamma<long double>(alpha + n);
		} catch (std::exception e){
			std::cout << e.what() << std::endl << "argument:" << (alpha + n);
		}

		term3 += n;
	}

	term2 += (dictionarySize - data.size()) * lgammaAlpha;
	long double lTerm3 = boost::math::lgamma<long double>(term3);

	return term2 - term1 - lTerm3;
}

//calculates the log-likelihood based on equation
long double App::calculateLogLikelihood2 (String2doubleMap data, int alpha, String2doubleMap beta)
{
	long double Sum_alphaBeta = 0;
	long double alphaBeta;
	long double Sum_alphaBetaN = 0;
	long double alphaBetaN;
	long double Sum_logAlphaBeta = 0;
	long double Sum_logAlphaBetaN = 0;
	std::string word;
	long double n;
	long double returnTerm;

	String2doubleMap::iterator iter;
	for (iter = beta.begin() ; iter != beta.end() ; ++iter)
	{
		//getting alpha * beta
		alphaBeta = (iter->second * alpha);

		//getting n
		word = iter->first;
		if (data.find(word) == data.end())
			n = 0;
		else
			n = data[word];
		
		alphaBetaN = n + alphaBeta;

		if (alphaBeta != 0)
		{
			Sum_alphaBeta += alphaBeta;
			Sum_logAlphaBeta += boost::math::lgamma<long double>(alphaBeta);
		}

		if (alphaBetaN != 0)
		{
			Sum_alphaBetaN += alphaBetaN;
			Sum_logAlphaBetaN += boost::math::lgamma<long double>(alphaBetaN);
		}

	}

	returnTerm = boost::math::lgamma<long double>(Sum_alphaBeta) - Sum_logAlphaBeta + Sum_logAlphaBetaN - boost::math::lgamma<long double>(Sum_alphaBetaN);

	return returnTerm;
}
//the initial probability for the first point
long double App::getInitialProbability()
{
	//we assume we start with a change point so returns 1

	if (isLogSpace)
		return 0;
	else
		return 1;
}

long double App::hazardFunction(bool isChangePoint)
{
	if (isChangePoint)
		return hazardRate;
	else
		return 1 - hazardRate;
}

long double App::logHazardFunction(bool isChangePoint)
{
	if (isChangePoint)
		return logHazardRate;
	else
		return log (1 - hazardRate);
}

std::vector <int> App::getMaxProbabilityindexAtEachTime(std::vector< std::vector <long double> > r)
{
	std::vector <long double> maxes;
	std::vector <int> maxIndecies;
	long double max;
	int maxIndex;

	for (unsigned int i = 0 ; i < r.size() ; ++i)
	{
		max = 0.0;
		maxIndex = -1;
		for (unsigned int j = 0 ; j < r.at(i).size() ; ++j)
		{
			if (r[i][j] > max)
			{
				max = r[i][j];
				maxIndex = j;
			}
		}

		maxes.push_back(max);
		maxIndecies.push_back(maxIndex);
	}

	return maxIndecies;
}

void App::testTokenizer()
{
	//std::string text = "token, test   string, M.B.A. Mr. Andrew hello.hi";

 //   boost::char_separator<char> sep(", ");
 //   boost::tokenizer< boost::char_separator<char> > tokens(text, sep);
 //   BOOST_FOREACH(const std::string& t, tokens) {
 //       std::cout << t << std::endl;
 //   }

	//std::string s = "This is,  a test M.B.A";
	//boost::tokenizer<> tok(s);
	//for(boost::tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg)
	//{
	//	std::cout << *beg << "\n";
	//}
}

/*
 * This method creates a simple 2D array of doubles that has two changepoints at t=6 and t=11
 */
std::vector< std::vector <long double> > App::makeTestData()
{
	std::vector <long double> eachRow;
	std::vector< std::vector <long double> > returnArray;
	long double offset = 0.1;

	//inserting {1.1, 1.1, 1.1, 1.1, 1.1} 5 times
	for (int i = 0 ; i < 5 ; ++i)
	{
		for (int j = 0 ; j < 5 ; ++j)
		{
			eachRow.push_back(1.1);
		}

		returnArray.push_back(eachRow);
		eachRow.clear();
	}

	//inserting a changepoint
	//inserting {10.10, 10.10, 10.10, 10.10, 10.10} 5 times
	for (int i = 0 ; i < 5 ; ++i)
	{
		for (int j = 0 ; j < 5 ; ++j)
		{
			eachRow.push_back(10.10);
		}

		returnArray.push_back(eachRow);
		eachRow.clear();
	}
	
	//inserting a changepoint
	//inserting {2.2, 2.2, 2.2, 2.2, 2.2} 5 times
	for (int i = 0 ; i < 5 ; ++i)
	{
		for (int j = 0 ; j < 5 ; ++j)
		{
			eachRow.push_back(2.2);
		}

		returnArray.push_back(eachRow);
		eachRow.clear();
	}

	return returnArray;
}

App::~App(void)
{
	delete 	ioHandler;
	delete 	preprocessHandle;
}
