#pragma once

#include "Definitions.h"

class Features
{
private:

	String2doubleMap wordCount;
	String2doubleMap POSCount;
	String2doubleMap functionWordCount;
	String2doubleMap stopWordCount;
	String2doubleMap bigramCount;
	String2doubleMap trigramCount;

public:

	Features(String2doubleMap wordCount, String2doubleMap POSCount, String2doubleMap functionWordCount, 
		String2doubleMap stopWordCount, String2doubleMap bigramCount, String2doubleMap trigramCount)
	{
		this->functionWordCount = functionWordCount;
		this->POSCount = POSCount;
		this->wordCount = wordCount;
		this->stopWordCount = stopWordCount;
		this->bigramCount = bigramCount;
		this->trigramCount = trigramCount;
	}

	//Features(String2doubleMap wordCount)
	//{
	//	this->wordCount = wordCount;
	//}

	//Features(String2doubleMap functionWordCount)
	//{
	//	this->functionWordCount = functionWordCount;
	//}

	//Features(String2doubleMap stopWordCount)
	//{
	//	this->stopWordCount = stopWordCount;
	//}

	String2doubleMap getWordCount() { return this->wordCount; }
	String2doubleMap getPOSCount() { return this->POSCount; }
	String2doubleMap getFunctionWordCount() { return this->functionWordCount; }
	String2doubleMap getStopWordCount() { return this->stopWordCount; }
	String2doubleMap getBigramCount() { return this->bigramCount; }
	String2doubleMap getTrigramCount() { return this->trigramCount; }
};