#pragma once

#include "Definitions.h"

class VectorAndMapUtility
{
public:

	VectorAndMapUtility(){}

	template<typename K , typename V> std::vector<K> getKeys(std::unordered_map<K, V> inputMap)
	{
		std::vector<K> keys;

		std::unordered_map<K, V>::iterator iter;

		for (iter = inputMap.begin() ; iter != inputMap.end() ; ++iter)
		{
			keys.push_back(iter->first);
		}

		return keys;
	}

	template<typename K , typename V> std::set<K> getUniqueKeys(std::unordered_map<K, V> inputMap)
	{
		std::set<K> keys;

		std::unordered_map<K, V>::iterator iter;

		for (iter = inputMap.begin() ; iter != inputMap.end() ; ++iter)
		{
			keys.insert(iter->first);
		}

		return keys;
	}

	template<typename K , typename V> std::vector<V> getValues(std::unordered_map<K, V> inputMap)
	{
		std::vector<V> values;

		std::unordered_map<K, V>::iterator iter;

		for (iter = inputMap.begin() ; iter != inputMap.end() ; ++iter)
		{
			values.push_back(iter->second);
		}

		return values;
	}

	//merges the maps in the input list of maps, if their index is between the start and end index
	template<typename K, typename V> std::unordered_map<K, V> 
	mergeMaps(std::vector< std::unordered_map<K, V> > inputList, int startIndex, int endIndex)
	{
		std::unordered_map<K, V> returnMap;
		std::unordered_map<K, V> currentMap;
		K currentKey;
		std::unordered_map<K, V>::iterator iter;

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

	template<typename K, typename V> V sumOfElements(std::unordered_map<K, V> inputMap)
	{
		V sum = 0;

		std::unordered_map<K, V>::iterator iter;

		for (iter = inputMap.begin() ; iter != inputMap.end() ; ++iter)
		{
			sum += iter->second;
		}

		return sum;
	}

	template<typename T> T sumOfElements(std::vector <T> inputVector)
	{
		T sum = 0;

		for (unsigned int i = 0 ; i < inputVector.size() ; ++i)
			sum += inputVector[i];

		return sum;
	}
};