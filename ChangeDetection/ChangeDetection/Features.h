#pragma once

#include "Definitions.h"

class Features
{
private:

	String2intMap wordCount;
	String2intMap POSCount;
	String2intMap functionWordCount;

public:

	Features(String2intMap wordCount, String2intMap POSCount, String2intMap functionWordCount)
	{
		this->functionWordCount = functionWordCount;
		this->POSCount = POSCount;
		this->wordCount = wordCount;
	}

	String2intMap getWordCount() { return this->wordCount; }
	String2intMap getPOSCount() { return this->POSCount; }
	String2intMap getFunctionWordCount() { return this->functionWordCount; }

};