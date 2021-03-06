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
	std::string lemmaOutputFilePath;

	for (unsigned int i = 0 ; i < fileNames.size() ; ++i)
	{
		std::cout << "\n\nfile name: " << fileNames[i];
		inputFilePath += fileNames[i];

		//run lemmatizer
		lemmaOutputFilePath = runLemmatizer(i, inputFilePath);

		//run POS Tagger/Tokenizer
		runPOSTagger(i, lemmaOutputFilePath);

		inputFilePath = filePath;
	}
}

std::string Preprocessing::runLemmatizer(int fileNumber, std::string inputFilePath)
{
	//running lemmatizer
	std::string command = 
	"C:\\Users\\andisheh\\Documents\\GitHub\\ChangeDetection\\ChangeDetection\\ChangeDetection\\LemmaGen_v2.0\\v2\\lemmagen\\binary\\win32\\release\\lemmatize.exe" ;
	//language model
	command += " -l C:\\Users\\andisheh\\Documents\\GitHub\\ChangeDetection\\ChangeDetection\\ChangeDetection\\LemmaGen_v2.0\\v2\\data\\lemmatizer\\lem-m-en.bin " ;
	//input file
	command += inputFilePath;
	//output file
	std::string outputFile = " C:\\Users\\andisheh\\Documents\\GitHub\\ChangeDetection\\ChangeDetection\\ChangeDetection\\lemma_output\\output";
	outputFile += "_" + std::to_string(static_cast<long long>(fileNumber)) + ".txt";
	command += outputFile;
	
	system(command.c_str());

	return outputFile;
}

Features* Preprocessing::getWordCountFeature(std::string text)
{
	String2doubleMap POSCount;
	String2doubleMap functionWordCount;
	String2doubleMap wordFrequencies;
	String2doubleMap stopWordCount;
	String2doubleMap bigramCount;
	String2doubleMap trigramCount;

	std::vector<std::string> allLines = strUtilHandle->split(text, '\n' );
	std::vector<std::string> allPairs;
	std::vector<std::string> bothWords;
	std::string POS;
	std::string word;

	std::cout << "\n\nstarted wordcount feature extraction...";

	//parsing stanford's output
	for (unsigned int i = 2 ; i < allLines.size() ; ++i)
	{
		allPairs = strUtilHandle->split(allLines[i], ' ' );

		for (unsigned int j = 0 ; j < allPairs.size() ; ++j)
		{
			bothWords = strUtilHandle->split(allPairs[j], '_' );

			word= strUtilHandle->toLowerCase(bothWords[0]);

			//word count for this document:
			if (!isElementInList(word, stopWords))
			{
				if (wordFrequencies.find(word) == wordFrequencies.end()) //if not in the map
					wordFrequencies[word] = 1;
				else
					wordFrequencies[word] = wordFrequencies[word] + 1;
			}

			//word count global:
			if (!isElementInList(word, stopWords))
			{
				if (globalWordCount.find(word) == globalWordCount.end()) //if not in the map
					globalWordCount[word] = 1;
				else
					globalWordCount[word] = globalWordCount[word] + 1;
			}
		}
	}

	std::cout << "\n\nDone.";

	return new Features(wordFrequencies, POSCount, functionWordCount, stopWordCount, bigramCount, trigramCount);
}

Features* Preprocessing::getBigramFeature(std::string text)
{
	String2doubleMap POSCount;
	String2doubleMap functionWordCount;
	String2doubleMap wordFrequencies;
	String2doubleMap stopWordCount;
	String2doubleMap bigramCount;
	String2doubleMap trigramCount;

	std::vector<std::string> allLines = strUtilHandle->split(text, '\n' );
	std::vector<std::string> allPairs;
	std::vector<std::string> bothWords;
	std::vector<std::string> bothWords1;
	std::vector<std::string> bothWords2;
	std::string word;
	std::string word1;
	std::string word2;
	std::string word3;
	bool found = false;

	std::cout << "\n\nstarted bigram feature extraction...";

	//parsing stanford's output
	for (unsigned int i = 2 ; i < allLines.size() - 1 ; ++i)
	{
		allPairs = strUtilHandle->split(allLines[i], ' ' );

		for (unsigned int j = 0 ; j < allPairs.size() - 1 ; ++j)
		{
			bothWords1 = strUtilHandle->split(allPairs[j], '_' );
			bothWords2 = strUtilHandle->split(allPairs[j+1], '_' );

			word1 = strUtilHandle->toLowerCase(bothWords1[0]);
			word2 = strUtilHandle->toLowerCase(bothWords2[0]);

			if (isElementInList(word1, stopWords))
				continue;
			else if (isElementInList(word2, stopWords))
			{
				int k = j + 2;
				while (k < allPairs.size())
				{
					bothWords = strUtilHandle->split(allPairs[k], '_' );
					word3 = strUtilHandle->toLowerCase(bothWords[0]);
					if (!isElementInList(word3, stopWords))
					{
						break;
					}
					else
						k++;
				}
				word = word1 + "_" + word3;
			}
			else
				word = word1 + "_" + word2;

			//word count for this document:
			if (bigramCount.find(word) == bigramCount.end()) //if not in the map
				bigramCount[word] = 1;
			else
				bigramCount[word] = bigramCount[word] + 1;

			//global count
			if (globalBigramCount.find(word) == globalBigramCount.end()) //if not in the map
				globalBigramCount[word] = 1;
			else
				globalBigramCount[word] = globalBigramCount[word] + 1;
		}
	}

	std::cout << "\n\nDone.";

	return new Features(wordFrequencies, POSCount, functionWordCount, stopWordCount, bigramCount, trigramCount);
}

Features* Preprocessing::getTrigramFeature(std::string text)
{
	String2doubleMap POSCount;
	String2doubleMap functionWordCount;
	String2doubleMap wordFrequencies;
	String2doubleMap stopWordCount;
	String2doubleMap bigramCount;
	String2doubleMap trigramCount;

	std::vector<std::string> allLines = strUtilHandle->split(text, '\n' );
	std::vector<std::string> allPairs;
	std::vector<std::string> bothWords;
	std::vector<std::string> bothWords1;
	std::vector<std::string> bothWords2;
	std::string word;
	std::string word2;
	bool found = false;

	std::cout << "\n\nstarted bigram feature extraction...";

	//parsing stanford's output
	for (unsigned int i = 2 ; i < allLines.size() ; ++i)
	{
		allPairs = strUtilHandle->split(allLines[i], ' ' );

		for (unsigned int j = 0 ; j < allPairs.size() - 1 ; ++j)
		{
			bothWords1 = strUtilHandle->split(allPairs[j], '_' );
			bothWords2 = strUtilHandle->split(allPairs[j+1], '_' );

			if (isElementInList(bothWords1[0], stopWords))
				continue;
			else if (isElementInList(bothWords2[0], stopWords))
			{
				int k = j + 2;
				while (k < allPairs.size())
				{
					bothWords = strUtilHandle->split(allPairs[k], '_' );
					if (!isElementInList(bothWords[0], stopWords))
					{
						word = bothWords1[0] + "_" + bothWords[0];
						break;
					}
					else
						k++;
				}
			}
			else
				word = bothWords1[0] + "_" + bothWords2[0];

			//word count for this document:
			if (bigramCount.find(word) == bigramCount.end()) //if not in the map
				bigramCount[word] = 1;
			else
				bigramCount[word] = bigramCount[word] + 1;

			//global count
			if (globalBigramCount.find(word) == globalBigramCount.end()) //if not in the map
				globalBigramCount[word] = 1;
			else
				globalBigramCount[word] = globalBigramCount[word] + 1;
		}
	}

	std::cout << "\n\nDone.";

	return new Features(wordFrequencies, POSCount, functionWordCount, stopWordCount, bigramCount, trigramCount);
}

Features* Preprocessing::getFunctionWordCountFeature(std::string text)
{
	String2doubleMap POSCount;
	String2doubleMap functionWordCount;
	String2doubleMap wordFrequencies;
	String2doubleMap stopWordCount;
	String2doubleMap bigramCount;
	String2doubleMap trigramCount;

	std::vector<std::string> allLines = strUtilHandle->split(text, '\n' );
	std::vector<std::string> allPairs;
	std::vector<std::string> bothWords;
	std::string word;
	std::string POS;

	std::cout << "\n\nstarted functionwordcount feature extraction...";

	//parsing stanford's output
	for (unsigned int i = 2 ; i < allLines.size() ; ++i)
	{
		allPairs = strUtilHandle->split(allLines[i], ' ' );

		for (unsigned int j = 0 ; j < allPairs.size() ; ++j)
		{
			bothWords = strUtilHandle->split(allPairs[j], '_' );

			word= strUtilHandle->toLowerCase(bothWords[0]);
			POS = bothWords[1];
			
			//functionWord count for this document:
			if (!isElementInList(POS, NON_FUNCTION_POS))
			{
				if (functionWordCount.find(word) == functionWordCount.end()) //if not in the map
					functionWordCount[word] = 1;
				else
					functionWordCount[word] = functionWordCount[word] + 1;
			}

			//functionWord count global:
			if (!isElementInList(POS, NON_FUNCTION_POS))
			{
				if (globalFunctionWordCount.find(word) == globalFunctionWordCount.end()) //if not in the map
					globalFunctionWordCount[word] = 1;
				else
					globalFunctionWordCount[word] = globalFunctionWordCount[word] + 1;
			}
		}
	}

	std::cout << "\n\nDone.";

	return new Features(wordFrequencies, POSCount, functionWordCount, stopWordCount, bigramCount, trigramCount);
}



Features* Preprocessing::getStopWordCountFeature(std::string text)
{
	String2doubleMap POSCount;
	String2doubleMap functionWordCount;
	String2doubleMap wordFrequencies;
	String2doubleMap stopWordCount;
	String2doubleMap bigramCount;
	String2doubleMap trigramCount;

	std::vector<std::string> allLines = strUtilHandle->split(text, '\n' );
	std::vector<std::string> allPairs;
	std::vector<std::string> bothWords;
	std::string POS;
	std::string word;

	std::cout << "\n\nstarted stopWordcount feature extraction...";

	//parsing stanford's output
	for (unsigned int i = 2 ; i < allLines.size() ; ++i)
	{
		allPairs = strUtilHandle->split(allLines[i], ' ' );

		for (unsigned int j = 0 ; j < allPairs.size() ; ++j)
		{
			bothWords = strUtilHandle->split(allPairs[j], '_' );

			word= strUtilHandle->toLowerCase(bothWords[0]);

			if (isElementInList(word, stopWords))
			{
				if (stopWordCount.find(word) == stopWordCount.end()) //if not in the map
					stopWordCount[word] = 1;
				else
					stopWordCount[word] = stopWordCount[word] + 1;
			}

			if (isElementInList(word, stopWords))
			{
				if (globalStopWordCount.find(word) == globalStopWordCount.end()) //if not in the map
					globalStopWordCount[word] = 1;
				else
					globalStopWordCount[word] = globalStopWordCount[word] + 1;
			}
		}
	}

	std::cout << "\n\nDone.";

	return new Features(wordFrequencies, POSCount, functionWordCount, stopWordCount, bigramCount, trigramCount);
}

Features* Preprocessing::getAllFeatures(std::string text)
{
	String2doubleMap POSCount;
	String2doubleMap functionWordCount;
	String2doubleMap wordFrequencies;
	String2doubleMap stopWordCount;
	String2doubleMap bigramCount;
	String2doubleMap trigramCount;

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

			word= strUtilHandle->toLowerCase(bothWords[0]);
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

	return new Features(wordFrequencies, POSCount, functionWordCount, stopWordCount, bigramCount, trigramCount);
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
int Preprocessing::getStopWordCountDictSize() { return globalStopWordCount.size(); }
int Preprocessing::getBigramDictSize() { return globalBigramCount.size(); }
int Preprocessing::getTrigramDictSize() { return globalTrigramCount.size(); }

String2doubleMap Preprocessing::getWordCountDict() { return this->globalWordCount; }
String2doubleMap Preprocessing::getFunctionWordCountDict() { return this->globalFunctionWordCount; }
String2doubleMap Preprocessing::getStopWordCountDict() { return this->globalStopWordCount; }
String2doubleMap Preprocessing::getBigramDict() { return this->globalBigramCount; }
String2doubleMap Preprocessing::getTrigramDict() { return this->globalTrigramCount; }

Preprocessing::~Preprocessing(void)
{
	delete 	strUtilHandle;
}
