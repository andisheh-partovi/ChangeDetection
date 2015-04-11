#include <boost\foreach.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

//data types
typedef std::unordered_map<std::string, double> String2doubleMap;
typedef std::vector <std::string> StringList;

//system constants
#define PROJECT_PATH "C:\\Users\\andisheh\\Documents\\GitHub\\ChangeDetection\\ChangeDetection\\ChangeDetection"
#define STANFORD_OUTPUT_PATH "C:\\Users\\andisheh\\Documents\\GitHub\\ChangeDetection\\ChangeDetection\\ChangeDetection\\stanford_output\\"
#define GAMMA 0.1666666 //used in the prioir probability over change P(r_t | r_t-1)
#define SIGNIFICANT_PROBABILITY_THRESHOLD 0.01 //the threshold unde which, probabilities should be ignored