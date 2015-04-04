#include <boost\foreach.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

//data types
typedef std::unordered_map<std::string, int> String2intMap;

enum Method
{
	wordCount,
	POSCount,
	functionWordCount
};

//system constants
#define PROJECT_PATH "C:\\Users\\andisheh\\Documents\\GitHub\\ChangeDetection\\ChangeDetection\\ChangeDetection"
#define GAMMA 0.1666666