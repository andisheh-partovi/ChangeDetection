#pragma once

#include "Definitions.h"

class Features
{
private:

	String2doubleMap wordCount;
	String2doubleMap POSCount;
	String2doubleMap functionWordCount;

public:

	Features(String2doubleMap wordCount, String2doubleMap POSCount, String2doubleMap functionWordCount)
	{
		this->functionWordCount = functionWordCount;
		this->POSCount = POSCount;
		this->wordCount = wordCount;
	}

	String2doubleMap getWordCount() { return this->wordCount; }
	String2doubleMap getPOSCount() { return this->POSCount; }
	String2doubleMap getFunctionWordCount() { return this->functionWordCount; }

};