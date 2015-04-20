#pragma once

#include "Definitions.h"

class Features
{
private:

	String2doubleMap wordCount;
	String2doubleMap POSCount;
	String2doubleMap functionWordCount;
	String2doubleMap stopWordCount;

public:

	Features(String2doubleMap wordCount, String2doubleMap POSCount, String2doubleMap functionWordCount, String2doubleMap stopWordCount)
	{
		this->functionWordCount = functionWordCount;
		this->POSCount = POSCount;
		this->wordCount = wordCount;
		this->stopWordCount = stopWordCount;
	}

	String2doubleMap getWordCount() { return this->wordCount; }
	String2doubleMap getPOSCount() { return this->POSCount; }
	String2doubleMap getFunctionWordCount() { return this->functionWordCount; }
	String2doubleMap getStopWordCount() { return this->stopWordCount; }
};