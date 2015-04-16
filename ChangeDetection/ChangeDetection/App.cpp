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

void App::run(Method method, DataSet dataSet, bool doParse, bool isLogSpace)
{
	std::cout << "started running\n\n";

	//------------------------------------------ 1.Preprocessing
	std::string dataFilesPath = PROJECT_PATH;
	switch(dataSet)
	{
	case TEST:
		dataFilesPath += "\\test_files\\";
		hazardRate = 0.1666666; //used in the prioir probability over change P(r_t | r_t-1) for synthetic data's test
		break;
	case STATE_OF_THE_UNION:
		dataFilesPath += "\\state_union\\";
		hazardRate = 0.1;
		break;
	default:
		std::cerr << "ERROR: unknown dataset selected.";
	}
	
	//feeding all the data files to the pre-process unit to get them processed
	StringList allDataFileNames = ioHandler->getAllFilesIndirectory(dataFilesPath);

	if (doParse)
		preprocessHandle->runTextPreprocessing( allDataFileNames , dataFilesPath);

	totalTimeSteps = allDataFileNames.size();

	//reading all the parsed files to grab their feature vectors
	for (unsigned int t = 0 ; t < totalTimeSteps ; ++t)
		allFeatues.push_back(preprocessHandle->getFeatures(ioHandler->getPOSTags(t)));

	this->method = method;
	this->isLogSpace = isLogSpace;

	//Setting dictionary size
	initializeChangeDetectionAlgorithm();

	////------------------------------------- 2.Running the algorithm
	if (isLogSpace)
		runLogChangeDetectionAlgorithm();
	else
		runChangeDetectionAlgorithm();

	std::cout << "finished running\n\n";

	//outputting:
	//writing the log:
	ioHandler->write2File(log, "log.txt");
	prin2DArray(r);

	//also print the maximums, which is what we actuallt want
	std::vector <int> maxes = getMaxProbabilityindexAtEachTime(r);
	print1DArray(maxes);

	//testing:
	std::cout << "\ndictionarySize: " << dictionary.size();
	//std::cout << "\n\n";
	//printString2intMap(utlityHandle->mergeMaps(allData, 0, allData.size()-1));
}

void App::feedData (String2doubleMap x_t)
{
	allDataSizes.push_back(utlityHandle->sumOfElements(x_t));
	allData.push_back(x_t);
	std::set<std::string> currentDictionary = utlityHandle->getUniqueKeys(x_t);
	dictionary.insert( currentDictionary.begin(), currentDictionary.end() );
	//printString2intMap(allData[allData.size() - 1]); //testing
}

//the initialization steps
void App::initializeChangeDetectionAlgorithm()
{
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
	for (unsigned int t = 1 ; t < totalTimeSteps ; ++t)
	{
		std::cout << "\ntimestep: " << t << "\n--------------------------------\n\n";
		log += "timestep: " + std::to_string(static_cast<long long>(t)) + ":\n";

		//Preprocess the datum to get the xt
		//the process can happen here or already happend in another module
		feedData(getx_t(t));

		//for all the points in r_t, calculate the joint probability
		for (unsigned int i = 0 ; i < t+1 ; ++i)
		{
			if ( i == 0) //possible changepoint : calcuate changepoint probability
			{
				P_rt_and_x_1_t = 0;

				//printString2intMap(normalize_x_t(allData[t])); std::cout<<"\n";//testing
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
					//printString2intMap(normalize_x_t(utlityHandle->mergeMaps(allData, t-i, t))); std::cout<<"\n";//testing
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
			log += "for point r_t = " + std::to_string(static_cast<long long>(i))
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
	long double likelihood;
	long double P_rt_and_x_1_t;
	std::vector<long double> joint_rt_probs;
	long double evidence;
	String2doubleMap x_t;

	//While there is a new datum available
	//for all files in the data folder
	for (unsigned int t = 1 ; t < 30 ; ++t)
	{
		std::cout << "\ntimestep: " << t << "\n--------------------------------\n\n";
		log += "timestep: " + std::to_string(static_cast<long long>(t)) + ":\n";

		//Preprocess the datum to get the xt
		//the process can happen here or already happend in another module
		feedData(getx_t(t));

		//for all the points in r_t, calculate the joint probability
		for (unsigned int i = 0 ; i < t+1 ; ++i)
		{
			if ( i == 0) //possible changepoint : calcuate changepoint probability
			{
				P_rt_and_x_1_t = 0;

				//printString2intMap(normalize_x_t(allData[t])); std::cout<<"\n";//testing
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
					//printString2intMap(normalize_x_t(utlityHandle->mergeMaps(allData, t-i, t))); std::cout<<"\n";//testing
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
			log += "for point r_t = " + std::to_string(static_cast<long long>(i))
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

//returns the datum at time t (x_t) in form of a vector
String2doubleMap App::getx_t (int t)
{
	switch (this->method)
	{
		case WORDCOUNT:
			return allFeatues.at(t)->getWordCount();
		case POSCOUNT:
			return allFeatues.at(t)->getPOSCount();
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

	return (/*constantFactor * */numerator) / boost::math::tgamma<long double>(denom);
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

//prints a 2D array of doubles
void App::prin2DArray(std::vector< std::vector <long double> > inputData)
{
	for (unsigned int i = 0 ; i < inputData.size() ; ++i)
	{
		for (unsigned int j = 0 ; j < inputData.at(i).size() ; ++j)
		{
			std::cout << inputData.at(i).at(j) << " , ";
		}

		std::cout << "\n";
	}
}

void App::print1DArray(std::vector <int> inputData)
{
	std::cout << "\n";

	for (unsigned int i = 0 ; i < inputData.size() ; ++i)
		std::cout << inputData.at(i)<< " , ";

	std::cout << "\n";
}

void App::printString2intMap(String2doubleMap inputMap)
{
	String2doubleMap::iterator iter;
	for (iter = inputMap.begin() ; iter != inputMap.end() ; ++iter)
	{
		std::cout << iter->first << ":" << iter->second << "\n";
	}
}

App::~App(void)
{
	delete 	ioHandler;
	delete 	preprocessHandle;
}
