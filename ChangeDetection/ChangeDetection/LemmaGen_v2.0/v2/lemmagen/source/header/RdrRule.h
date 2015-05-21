/******************************************************************************
This file is part of the lemmagen library. It gives support for lemmatization.
Copyright (C) 2006-2007 Matjaz Jursic <matjaz@gmail.com>

The lemmagen library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/
#pragma once

#include "RdrLemmatizer.h"
#include "MiscLib.h"

class RdrRule{

public:
	//reference counter
	int iObjectCounter;

	//byte *acFrom;
	byte *acTo;
	int iFromLen;
	int iToLen;

	//data for usage when creating lemmatizer binary representation
	int iAddress;

public:
	RdrRule();
	RdrRule(int iFromLen, byte *acTo);
	~RdrRule();

	char *ToString(const char *acSuffix, const char *acArrow="-->") const;
	char *UniqueStringId(char *acAppend = NULL) const;

	char *Lemmatize(const char *acWord, int iWordLen=-1) const;

	void SetAddress(int &iAddr, bool bReset);
	void SetByteCodeThis(byte *abData, bool bEntireWord);

private:
	int GetByteLength();

};
