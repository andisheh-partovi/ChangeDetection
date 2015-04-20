#pragma once

#include "Definitions.h"

#include <dirent.h>
#include <fstream>

class IOHandler
{
public:
	IOHandler(void);
	~IOHandler(void);

	//generic
	std::string readFile(std::string filePath);
	void write2File(std::string text, std::string filePath);
	StringList getAllFilesIndirectory(std::string directoryPath);
	
	//project specific
	std::string fileNUmber2FilePath (int fileNumber);
	std::string getPOSTags(int fileNumber);

	//screen output:
	template<typename T> void print1DArray(std::vector <T> inputData)
	{
		std::cout << "\n";

		for (unsigned int i = 0 ; i < inputData.size() ; ++i)
			std::cout << inputData.at(i)<< " , ";

		std::cout << "\n";
	}

	template<typename K, typename V> void printMap(std::unordered_map<K, V> inputMap)
	{
		std::unordered_map<K, V>::iterator iter;

		for (iter = inputMap.begin() ; iter != inputMap.end() ; ++iter)
			std::cout << iter->first << ":" << iter->second << "\n";
	}

	template<typename T> void print2DArray(std::vector< std::vector <T> > inputData)
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

};

