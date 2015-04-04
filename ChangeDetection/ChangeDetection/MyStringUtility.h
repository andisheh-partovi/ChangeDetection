#ifndef MY_STRING_UTILITY_H
#define MY_STRING_UTILITY_H

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>

class MyStringUtility
{

public:

	//example: vector<string> splittedStr = strObj->split("Andisheh is a brilliant programmer", ' ' );
	std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) 
	{
		std::stringstream ss(s);
		std::string item;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}
		return elems;
	}


	std::vector<std::string> split(const std::string &s, char delim) 
	{
		std::vector<std::string> elems;
		split(s, delim, elems);
		return elems;
	}

	std::string& remove_chars(std::string& s, const std::string& chars) 
	{
		s.erase(std::remove_if(s.begin(), s.end(), [&chars](const char& c) {
			return chars.find(c) != std::string::npos;
		}), s.end());
		return s;
	}

	std::string remove_chars_copy(std::string s, const std::string& chars)
	{
		return remove_chars(s, chars);
	}

	std::string& remove_nondigit(std::string& s) 
	{
		s.erase(std::remove_if(s.begin(), s.end(), [](const char& c) {
			return !isdigit(c);
		}), s.end());
		return s;
	}

	std::string remove_nondigit_copy(std::string s) 
	{
		return remove_nondigit(s);
	}

	std::string& remove_chars_if_not(std::string& s, const std::string& allowed) 
	{
		s.erase(std::remove_if(s.begin(), s.end(), [&allowed](const char& c) {
			return allowed.find(c) == std::string::npos;
		}), s.end());
		return s;
	}

	std::string remove_chars_if_not_copy(std::string s, const std::string& allowed) 
	{
		return remove_chars_if_not(s, allowed);
	}

	std::string removeFirsOccurenceOfWord(std::string inputString, std::string wordToBeRemoved)
	{
		std::string outputString (inputString);
		int pos = inputString.find(wordToBeRemoved);

		if( pos != std::string::npos )
		{
			outputString.swap(outputString.erase(pos,wordToBeRemoved.length()));
		}

		return outputString;
	}

	bool hasWord (std::string inputString, std::string wordToFind)
	{
		int pos = inputString.find(wordToFind);

		if( pos != std::string::npos )
			return true;

		return false;
	}

	std::string toLowerCase(std::string inputString)
	{
		std::string retrunString (inputString);
		for(unsigned int i = 0 ; i < inputString.length() ; ++i)
			retrunString[i] = tolower(inputString[i]);

		return retrunString;
	}

	std::string removeAllOccurencesOfCharacter(std::string inputString, char charToBeRemoved)
	{
		std::remove(inputString.begin(), inputString.end(), charToBeRemoved);
	}

	int string2Int (std::string inputString)
	{
		return std::stoi(inputString);
	}

};


#endif //end of MY_STRING_UTILITY_H