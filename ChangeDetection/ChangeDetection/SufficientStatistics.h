#pragma once

#include "Definitions.h"

class SufficientStatistics
{
public:
	int alpha;
	String2doubleMap beta;

	SufficientStatistics(int alpha, String2doubleMap beta)
	{
		this->alpha = alpha;
		this->beta = beta;
	}
};