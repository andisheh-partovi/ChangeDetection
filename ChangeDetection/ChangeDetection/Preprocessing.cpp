#include "Preprocessing.h"

#include <windows.h>//SetCurrentDirectory

Preprocessing::Preprocessing(void)
{	
	strUtilHandle = new MyStringUtility();

	//defining the non_function_POSs:
	//adjectives
	NON_FUNCTION_POS.push_back("JJ");
	NON_FUNCTION_POS.push_back("JJR");
	NON_FUNCTION_POS.push_back("JJS");
	//nouns
	NON_FUNCTION_POS.push_back("NN");
	NON_FUNCTION_POS.push_back("NNP");
	NON_FUNCTION_POS.push_back("NNPS");
	NON_FUNCTION_POS.push_back("NNS");
	//adverbs
	NON_FUNCTION_POS.push_back("RB");
	NON_FUNCTION_POS.push_back("RBR");
	NON_FUNCTION_POS.push_back("RBS");
	//verbs
	NON_FUNCTION_POS.push_back("VB");
	NON_FUNCTION_POS.push_back("VBD");
	NON_FUNCTION_POS.push_back("VBG");
	NON_FUNCTION_POS.push_back("VBN");
	NON_FUNCTION_POS.push_back("VBP");
	NON_FUNCTION_POS.push_back("VBZ");
}

void Preprocessing::runPOSTagger(int fileNumber, std::string inputFilePath)
{
	std::string outputFilePath = STANFORD_OUTPUT_PATH;
	outputFilePath += "stanford_output" + std::to_string(static_cast<long long>(fileNumber)) + ".txt";

	std::string command = "stanford-postagger models\\wsj-0-18-left3words-distsim.tagger ";
	command += inputFilePath + " > " + outputFilePath;

	//std::cout << "\n\ncommand: " << command << "\n\n";

	std::string currentDirectory = PROJECT_PATH;
	currentDirectory += "\\stanford-postagger-full-2013-06-20";
	SetCurrentDirectory(currentDirectory.c_str());
	system(command.c_str());

	SetCurrentDirectory(PROJECT_PATH);
}

void Preprocessing::runTextPreprocessing(StringList fileNames, std::string filePath)
{
	std::string inputFilePath = filePath;

	for (unsigned int i = 0 ; i < fileNames.size() ; ++i)
	{
		std::cout << "\n\nfile name: " << fileNames[i];
		inputFilePath += fileNames[i];
		runPOSTagger(i, inputFilePath);
		inputFilePath = filePath;
	}
}

Features* Preprocessing::getFeatures(std::string text)
{
	String2doubleMap POSCount;
	String2doubleMap functionWordCount;
	String2doubleMap wordFrequencies;
	String2doubleMap stopWordCount;

	std::vector<std::string> allLines = strUtilHandle->split(text, '\n' );
	std::vector<std::string> allPairs;
	std::vector<std::string> bothWords;
	std::string POS;
	std::string word;

	std::cout << "\n\nstarted feature extraction...";

	//parsing stanford's output
	for (unsigned int i = 2 ; i < allLines.size() ; ++i)
	{
		allPairs = strUtilHandle->split(allLines[i], ' ' );

		for (unsigned int j = 0 ; j < allPairs.size() ; ++j)
		{
			bothWords = strUtilHandle->split(allPairs[j], '_' );

			word= bothWords[0];
			POS = bothWords[1];

			//POS count for this document:
			if (POSCount.find(POS) == POSCount.end()) //if not in the map
				POSCount[POS] = 1;
			else
				POSCount[POS] = POSCount[POS] + 1;

			//POS count global:
			//if (globalPOSCount.find(POS) == globalPOSCount.end()) //if not in the map
			//	globalPOSCount[POS] = 1;
			//else
			//	globalPOSCount[POS] = globalPOSCount[POS] + 1;

			//functionWord count for this document:
			if (!isElementInList(POS, NON_FUNCTION_POS))
			{
				if (functionWordCount.find(word) == functionWordCount.end()) //if not in the map
					functionWordCount[word] = 1;
				else
					functionWordCount[word] = functionWordCount[word] + 1;
			}

			//functionWord count global:
			//if (!isElementInList(POS, NON_FUNCTION_POS))
			//{
			//	if (globalFunctionWordCount.find(word) == globalFunctionWordCount.end()) //if not in the map
			//		globalFunctionWordCount[word] = 1;
			//	else
			//		globalFunctionWordCount[word] = globalFunctionWordCount[word] + 1;
			//}

			//word count for this document:
			if (!isElementInList(word, stopWords))
			{
				if (wordFrequencies.find(word) == wordFrequencies.end()) //if not in the map
					wordFrequencies[word] = 1;
				else
					wordFrequencies[word] = wordFrequencies[word] + 1;
			}
			//word count global:
			//if (globalWordCount.find(word) == globalWordCount.end()) //if not in the map
			//	globalWordCount[word] = 1;
			//else
			//	globalWordCount[word] = globalWordCount[word] + 1;

			//stopword counts for this doc:
			if (isElementInList(word, stopWords))
			{
				if (stopWordCount.find(word) == stopWordCount.end()) //if not in the map
					stopWordCount[word] = 1;
				else
					stopWordCount[word] = stopWordCount[word] + 1;
			}
		}
	}

	std::cout << "\n\nDone.";

	return new Features(wordFrequencies, POSCount, functionWordCount, stopWordCount);
}

//checks if the element is included at least once in the list or not
bool Preprocessing::isElementInList(std::string element, StringList list)
{
	for (unsigned int i = 0 ; i < list.size() ; ++i)
		if (list.at(i).compare(element) == 0)
			return true;

	return false;
}

void Preprocessing::setStopWordsList(std::string inputFileContent)
{
	stopWords = strUtilHandle->split(inputFileContent, '\n' );
}

StringList Preprocessing::getStopWords() { return this->stopWords; }
int Preprocessing::getWordCountDictSize() { return globalWordCount.size(); }
int Preprocessing::getPOSCountDictSize() { return globalPOSCount.size(); }
int Preprocessing::getFunctionWordCountDictSize() { return globalFunctionWordCount.size(); }

Preprocessing::~Preprocessing(void)
{
	delete 	strUtilHandle;
}
