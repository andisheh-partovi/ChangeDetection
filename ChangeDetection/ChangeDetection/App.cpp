#include "App.h"

#include <boost\tokenizer.hpp>
#include <boost/lexical_cast.hpp>//casting long double to string
#include <boost/math/special_functions/gamma.hpp> //gamma function (tgamma)
#include <math.h> //pow


App::App(void)
{
	ioHandler = new IOHandler();
	preprocessHandle = new Preprocessing();
}

void App::run(Method method)
{
	std::cout << "started running\n\n";

	//------------------------------------------ 1.Preprocessing
	std::string dataFilesPath = PROJECT_PATH;
	//dataFilesPath += "\\test_files\\";
	dataFilesPath += "\\state_union\\";
	
	//feeding all the data files to the pre-process unit to get them processed
	StringList allDataFileNames = ioHandler->getAllFilesIndirectory(dataFilesPath);
	//preprocessHandle->runTextPreprocessing( allDataFileNames , dataFilesPath);

	totalTimeSteps = allDataFileNames.size();

	//reading all the parsed files to grab their feature vectors
	for (unsigned int t = 0 ; t < totalTimeSteps ; ++t)
		allFeatues.push_back(preprocessHandle->getFeatures(ioHandler->getPOSTags(t)));

	//Setting dictionary size
	initializeChangeDetectionAlgorithm(method);
	switch (method)
	{
	case wordCount:
		dictionarySize = preprocessHandle->getWordCountDictSize();
		break;
	case POSCount:
		dictionarySize = preprocessHandle->getPOSCountDictSize();
		break;
	case functionWordCount:
		dictionarySize = preprocessHandle->getFunctionWordCountDictSize();
		break;
	default:
			std::cerr << "unkown method selected:" << boost::lexical_cast<std::string>(method);
	}

	////------------------------------------- 2.Running the algorithm
	runChangeDetectionAlgorithm(method);

	std::cout << "finished running\n\n";

	//outputting:
	//writing the log:
	ioHandler->write2File(log, "log.txt");
	prin2DArray(r);

	//testing:
	std::cout << "\ndictionarySize: " << dictionarySize;
	//std::cout << "\n\n";
	//printString2intMap(mergeString2intMaps(allData, 0, allData.size()-1));
}

//the initialization steps
void App::initializeChangeDetectionAlgorithm(Method method)
{
	//setting the first element of the r matrix to its initial value
	std::vector<long double> tempRow;
	tempRow.push_back(getInitialProbability());
	r.push_back(tempRow);
	
	//reading and saving the first datum
	allData.push_back(getx_t(0, method));
}

void App::runChangeDetectionAlgorithm(Method method)
{
	long double likelihood;
	long double P_rt_and_x_1_t;
	std::vector<long double> joint_rt_probs;
	long double evidence;

	//While there is a new datum available
	//for all files in the data folder
	for (unsigned int t = 1 ; t < totalTimeSteps ; ++t)
	{
		std::cout << "\ntimestep: " << t << "\n--------------------------------\n\n";
		log += "timestep: " + std::to_string(static_cast<long long>(t)) + ":\n";

		//Preprocess the datum to get the xt
		//the process can happen here or already happend in another module
		allData.push_back(getx_t(t, method));
		//printString2intMap(allData[t]); //testing

		//for all the points in r_t, calculate the joint probability
		for (unsigned int i = 0 ; i < t+1 ; ++i)
		{
			if ( i == 0) //possible changepoint : calcuate changepoint probability
			{
				P_rt_and_x_1_t = 0;

				likelihood = calculateLikelihood(allData[t], dictionarySize);

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
					//printString2intMap(mergeString2intMaps(allData, t-i, t)); std::cout<<"\n";//testing
					likelihood = calculateLikelihood(mergeString2intMaps(allData, t-i, t), dictionarySize);
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
		evidence = sumOfElements(joint_rt_probs);

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
String2intMap App::getx_t (int t, Method method)
{
	switch (method)
	{
		case wordCount:
			return allFeatues.at(t)->getWordCount();
		case POSCount:
			return allFeatues.at(t)->getPOSCount();
		case functionWordCount:
			return allFeatues.at(t)->getFunctionWordCount();
		default:
			std::cerr << "ERROR: unkown method selected:" << boost::lexical_cast<std::string>(method);
	}
}

//merges the maps in the input list of maps, if their index is between the start and end index
String2intMap App::mergeString2intMaps(std::vector< String2intMap > inputList, int startIndex, int endIndex)
{
	String2intMap returnMap;
	String2intMap currentMap;
	String2intMap::iterator iter;
	std::string currentKey;

	for(int i = startIndex ; i <= endIndex ; ++i) 
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
long double App::sumOfElements(std::vector <long double> inputVector)
{
	long double sum = 0;

	for (unsigned int i = 0 ; i < inputVector.size() ; ++i)
		sum += inputVector[i];

	return sum;
}


//original likelihood method
//long double App::calculateLikelihood (String2intMap x_t, int dictionarySize)
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
//	String2intMap::iterator iter;
//	for (iter = x_t.begin() ; iter != x_t.end() ; ++iter)
//	{
//		numerator *= boost::math::tgamma<long double>(alpha + iter->second);
//		denom += alpha + iter->second;
//	}
//
//	return (constantFactor * numerator) / boost::math::tgamma<long double>(denom);
//}

long double App::calculateLikelihood (String2intMap data, int dictionarySize)
{
	long double alpha = 1.0/dictionarySize;
	long double gammaAlpha = boost::math::lgamma<long double>(alpha);

	long double constantFactor = boost::math::lgamma<long double>(1) / pow(gammaAlpha, dictionarySize);

	long double numerator = 1;
	long double denom = 0;

	String2intMap::iterator iter;
	for (iter = data.begin() ; iter != data.end() ; ++iter)
	{
		//if ((alpha + iter->second) > 100)
			//std::cout << "\n\nword: X" << iter->first << "X\n\n";
		//else
		//{
			try{
			numerator *= boost::math::lgamma<long double>(alpha + iter->second);
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

	return (constantFactor * numerator) / boost::math::lgamma<long double>(denom);
}

//the initial probability for the first point
long double App::getInitialProbability()
{
	//we assume we start with a change point so returns 1
	return 1;
}

long double App::hazardFunction(bool isChangePoint)
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
	delete 	ioHandler;
	delete 	preprocessHandle;
}
