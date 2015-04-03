#include "App.h"

#include <boost\tokenizer.hpp>
#include <boost/math/special_functions/gamma.hpp> //gamma function (tgamma)
#include <math.h> //pow


App::App(void)
{
	ioHandler = new IOHandler();

	dictionarySize = 24;
}

void App::run()
{
	std::cout << "started running\n\n";

	//initialization step
	initializeChangeDetectionAlgorithm();

	runChangeDetectionAlgorithm();

	std::cout << "finished running\n";
}

//the initialization steps
void App::initializeChangeDetectionAlgorithm()
{
	//setting the first element of the r matrix to its initial value
	std::vector<double> tempRow;
	tempRow.push_back(getInitialProbability());
	r.push_back(tempRow);
	
	//reading and saving the first datum
	allData.push_back(getx_t(0));
}

void App::runChangeDetectionAlgorithm()
{
	String2intMap x_t;
	double likelihood;
	double P_rt_and_x_1_t;
	std::vector<double> joint_rt_probs;
	double evidence;

	//While there is a new datum available
	//for all files in the data folder
	for (unsigned int t = 1 ; t < 5 ; ++t)
	{
		std::cout << "timestep: " << t << "\n";

		//Preprocess the datum to get the xt
		//the process can happen here or already happend in another module
		x_t = getx_t(t);
		allData.push_back(x_t);
		//printString2intMap(x_t);

		//for all the points in r_t, calculate the joint probability
		for (unsigned int i = 0 ; i < t+1 ; ++i)
		{
			if ( i == 0) //possible changepoint : calcuate changepoint probability
			{
				P_rt_and_x_1_t = 0;

				likelihood = calculateLikelihood(x_t, dictionarySize);

				//for all the points in r_t-1
				for (unsigned int j = 0 ; j < r.at(t-1).size() ; ++j)
				{
					P_rt_and_x_1_t += r[t-1][j] * likelihood * hazardFunction(true);
				}
			}
			else //possible continuation : calcuate Growth probability
			{
				likelihood = calculateLikelihood(mergeString2intMaps(allData, 0, i-1), dictionarySize);
				P_rt_and_x_1_t = r[t-1][i-1] * likelihood * hazardFunction(false);
			}

			//save the joint probabilities
			joint_rt_probs.push_back(P_rt_and_x_1_t);
		}

		//for all the points in r_t, calculate the conditional probability
		//first get the sum (evidence probability)
		evidence = sumOfElements(joint_rt_probs);

		//then divide all the joints by the sum
		for (unsigned int i = 0 ; i < t+1 ; ++i)
		{
			joint_rt_probs.at(i) = joint_rt_probs.at(i) / evidence;
		}

		r.push_back(joint_rt_probs);
		joint_rt_probs.clear();
	}
}

//returns the datum at time t (x_t) in form of a vector
String2intMap App::getx_t (int t)
{
	std::string filePath = "C:\\Users\\andisheh\\Documents\\GitHub\\ChangeDetection\\ChangeDetection\\ChangeDetection\\test files\\"
							+ std::to_string(static_cast<long long>(t)) + ".txt";

	return getWordFrequencyCount(ioHandler->readFile(filePath));
}

//merges the maps in the input list of maps, if their index is between the start and end index
String2intMap App::mergeString2intMaps(std::vector< String2intMap > inputList, int startIndex, int endIndex)
{
	String2intMap returnMap;
	String2intMap currentMap;
	String2intMap::iterator iter;
	std::string currentKey;

	for(unsigned int i = startIndex ; i <= endIndex ; ++i) 
	{
		currentMap = inputList.at(i);

		for (iter = currentMap.begin() ; iter != currentMap.end() ; ++iter)
		{
			currentKey = iter->first;
			if (returnMap.find(currentKey) == returnMap.end()) //if not in the map
				returnMap[currentKey] = 1;
			else
				returnMap[currentKey] = returnMap[currentKey] + 1;
		}
    }

	return returnMap;
}

//returns a map of words and their frequency count
String2intMap App::getWordFrequencyCount(std::string inputText)
{
	String2intMap wordFrequencies;

	//1. tokenize the input string

	boost::char_separator<char> sep(" ");
    boost::tokenizer< boost::char_separator<char> > tokens(inputText, sep);

	//2. count the word frequencies and fill the vector
	BOOST_FOREACH(const std::string& t, tokens) 
	{
		if (wordFrequencies.find(t) == wordFrequencies.end()) //if not in the map
			wordFrequencies[t] = 1;
		else
			wordFrequencies[t] = wordFrequencies[t] + 1;
    }

	//return hash2Vector(wordFrequencies);// if only interested in the count and not the whole map
	return wordFrequencies;
}

std::vector <int> App::hash2Vector(String2intMap inputMap)
{
	std::vector <int> returnList;

	String2intMap::iterator iter;

	for (iter = inputMap.begin() ; iter != inputMap.end() ; ++iter)
	{
		returnList.push_back(iter->second);
	}

	return returnList;
}

//returns the sum of all the elements in the input vector of doubles
double App::sumOfElements(std::vector <double> inputVector)
{
	double sum = 0;

	for (int i = 0 ; i < inputVector.size() ; ++i)
		sum += inputVector[i];

	return sum;
}

double App::calculateLikelihood (String2intMap x_t, int dictionarySize)
{
	int D = x_t.size();
	double alpha = 1.0/dictionarySize;
	double gammaAlpha = boost::math::tgamma<double>(alpha);

	double constantFactor = boost::math::tgamma<double>(alpha * D) / pow(gammaAlpha, D);

	double numerator = 1;
	double denom = 0;

	String2intMap::iterator iter;
	for (iter = x_t.begin() ; iter != x_t.end() ; ++iter)
	{
		numerator *= boost::math::tgamma<double>(alpha + iter->second);
		denom += alpha + iter->second;
	}

	return (constantFactor * numerator) / boost::math::tgamma<double>(denom);
}

//the initial probability for the first point
double App::getInitialProbability()
{
	//we assume we start with a change point so returns 1
	return 1;
}

double App::hazardFunction(bool isChangePoint)
{
	if (isChangePoint)
		return GAMMA;
	else
		return 1 - GAMMA;
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
std::vector< std::vector <double> > App::makeTestData()
{
	std::vector <double> eachRow;
	std::vector< std::vector <double> > returnArray;
	double offset = 0.1;

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
void App::prin2DArray(std::vector< std::vector <double> > inputData)
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

void App::printString2intMap(String2intMap inputMap)
{
	String2intMap::iterator iter;
	for (iter = inputMap.begin() ; iter != inputMap.end() ; ++iter)
	{
		std::cout << iter->first << ":" << iter->second << "\n";
	}
}

App::~App(void)
{
}
