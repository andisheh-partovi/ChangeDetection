#include "IOHandler.h"


IOHandler::IOHandler(void)
{
}

std::string IOHandler::readFile(std::string filePath)
{
	std::string fileContent;
	std::string eachLine;
	std::ifstream file;

	file.open (filePath);

	if (file.is_open())
	{
		while(!file.eof()) // To get you all the lines.
		{
			std::getline(file,eachLine); // Saves the line in STRING.
			fileContent += eachLine + "\n";
		}

		file.close();

		return fileContent;
	}
	else
	{
		std::cerr << "unable to open file for reading:" << filePath;
		return NULL;
	}
}

//writes the input "text" to the files with the specified path
void IOHandler::write2File(std::string text, std::string filePath)
{
	std::ofstream file;
	file.open (filePath);

	if (file.is_open())
	{
		file << text;

		file.close();

	}
	else
		std::cerr << "unable to open file for writing:" << filePath;
}

//returns a list of all the files and folders in the input directory
StringList IOHandler::getAllFilesIndirectory(std::string directoryPath)
{
	DIR *dir;
	struct dirent *ent;
	StringList returnList;

	//we want to exclude . and .. from the list later
	std::string dot = ".";
	std::string doubleDot = "..";

	if ((dir = opendir (directoryPath.c_str())) != NULL) 
	{
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) 
		{
			if (dot.compare(ent->d_name) != 0 && doubleDot.compare(ent->d_name) != 0)
			{
				returnList.push_back(ent->d_name);
				std::cout << ent->d_name << "\n";
			}
		}
		closedir (dir);

	} 
	else //could not open directory
		std::cerr << "Could not open directory to list: " << directoryPath;

	return returnList;
}

std::string IOHandler::fileNUmber2FilePath (int fileNumber)
{
	std::string testLocation = "\\test_files\\";
	return PROJECT_PATH + testLocation + std::to_string(static_cast<long long>(fileNumber)) + ".txt";
}

std::string IOHandler::getPOSTags(int fileNumber)
{
	std::string filePath = STANFORD_OUTPUT_PATH;
	filePath += "stanford_output" + std::to_string(static_cast<long long>(fileNumber)) + ".txt";

	return readFile(filePath);
}

IOHandler::~IOHandler(void)
{
}
