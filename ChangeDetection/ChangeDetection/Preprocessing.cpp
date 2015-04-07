#include "Preprocessing.h"


Preprocessing::Preprocessing(void)
{
}

void Preprocessing::runPOSTagger(int fileNumber, std::string inputFilePath)
{
	std::string outputFilePath = STANFORD_OUTPUT_PATH;
	outputFilePath = "stanford_output" + std::to_string(static_cast<long long>(fileNumber)) + ".txt";

	std::string command = "stanford-postagger models\\wsj-0-18-left3words-distsim.tagger ";
	command += inputFilePath + " > " + outputFilePath;

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
		inputFilePath += fileNames[i];
		runPOSTagger(i, inputFilePath);
		inputFilePath = filePath;
	}
}


Preprocessing::~Preprocessing(void)
{
}
