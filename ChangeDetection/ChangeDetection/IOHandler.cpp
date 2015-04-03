#include "IOHandler.h"


IOHandler::IOHandler(void)
{
}

std::string IOHandler::readFile(std::string filePath)
{
	std::string fileContent;
	std::ifstream file;

	file.open (filePath);

	if (file.is_open())
	{
		while(!file.eof()) // To get you all the lines.
		{
			std::getline(file,fileContent); // Saves the line in STRING.
		}

		file.close();

		return fileContent;
	}
	else
	{
		std::cerr << "unable to open file" << filePath;
		return NULL;
	}
}


IOHandler::~IOHandler(void)
{
}
