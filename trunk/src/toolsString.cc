/***************************************************************************
 *            toolsString.cc
 *
 *  Sun Oct 30 11:44:19 2007
 *  Copyright  2007  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include <libdodo/toolsString.h>

using namespace dodo::tools;

bool
string::equal(const dodoString &first,
	      const dodoString &second)
{
	unsigned long firstSize = first.size();

	if (firstSize != second.size())
		return false;

	if (firstSize == 0)
		return true;

	for (unsigned long i = 0; i < firstSize; ++i)
		if (first[i] != second[i])
			return false;

	return true;
}

//-------------------------------------------------------------------

bool
string::iequal(const dodoString &first,
	       const dodoString &second)
{
	unsigned long firstSize = first.size();

	if (firstSize != second.size())
		return false;

	if (firstSize == 0)
		return true;

	for (unsigned long i = 0; i < firstSize; ++i)
		if (tolower(first[i]) != tolower(second[i]))
			return false;

	return true;
}

//-------------------------------------------------------------------

bool
string::contains(const dodoString &str,
		 const dodoString &needle,
		 bool icase)
{
	if (icase)
	{
		if (strcasestr(str.c_str(), needle.c_str()) != NULL)
			return true;
	}
	else
	{
		if (strstr(str.c_str(), needle.c_str()) != NULL)
			return true;
	}

	return false;
}

//-------------------------------------------------------------------

unsigned long
string::find(const dodoString &str,
	     const dodoString &needle,
	     unsigned long position,
	     bool icase)
{
	if (position > str.size())
		return dodoString::npos;

	char *pos = NULL;

	if (icase)
	{
		if ((pos = strcasestr(str.c_str() + position, needle.c_str())) == NULL)
			return dodoString::npos;
		else
			return(pos - str.c_str());
	}
	else
	{
		if ((pos = strstr(str.c_str() + position, needle.c_str())) == NULL)
			return dodoString::npos;
		else
			return(pos - str.c_str());
	}

	return dodoString::npos;
}

//-------------------------------------------------------------------

unsigned long
string::find(const dodoString &str,
	     const dodoString &needle,
	     bool icase)
{
	return find(str, needle, 0, icase);
}

//-------------------------------------------------------------------

dodoString
string::format(const dodoString &format, ...)
{
	unsigned long length = format.size() * 3;
	char *str = new char[length + 1];

	va_list ap;

	va_start(ap, format);
	vsnprintf(str, length, format.c_str(), ap);
	va_end(ap);

	dodoString res = str;

	delete [] str;

	return res;
}

//-------------------------------------------------------------------

dodoString
string::lToString(long number)
{
	char temp[SIZEOFNUM];
	sprintf(temp, "%ld", number);

	return temp;
}

//-------------------------------------------------------------------

dodoString
string::ulToString(unsigned long number)
{
	char temp[SIZEOFNUM];
	sprintf(temp, "%ld", number);

	return temp;
}

//-------------------------------------------------------------------

dodoString
string::iToString(int number)
{
	char temp[SIZEOFNUM];
	sprintf(temp, "%d", number);

	return temp;
}

//-------------------------------------------------------------------

dodoString
string::uiToString(unsigned int number)
{
	char temp[SIZEOFNUM];
	sprintf(temp, "%u", number);

	return temp;
}

//-------------------------------------------------------------------

dodoString
string::sToString(short number)
{
	char temp[SIZEOFNUM];
	sprintf(temp, "%sd", number);

	return temp;
}

//-------------------------------------------------------------------

dodoString
string::usToString(unsigned short number)
{
	char temp[SIZEOFNUM];
	sprintf(temp, "%sd", number);

	return temp;
}


//-------------------------------------------------------------------

dodoString
string::fToString(float number)
{
	char temp[SIZEOFNUM];
	sprintf(temp, "%f", number);

	return temp;
}

//-------------------------------------------------------------------

dodoString
string::dToString(double number)
{
	char temp[SIZEOFNUM];
	sprintf(temp, "%f", number);

	return temp;
}

//-------------------------------------------------------------------

dodoString
string::lTrim(const dodoString &data,
	      char symbol)
{
	int size = data.size(), i(0);

	for (; i < size; ++i)
		if (data[i] != symbol)
			break;

	return data.substr(i, size - i);
}

//-------------------------------------------------------------------

dodoString
string::rTrim(const dodoString &data,
	      char symbol)
{
	int i(data.size() - 1);

	for (; i >= 0; --i)
		if (data[i] != symbol)
			break;

	return data.substr(0, i + 1);
}

//-------------------------------------------------------------------

dodoString
string::rTrim(const dodoString &data,
	      const char symbols[],
	      int symCount)
{
	int i(data.size() - 1), j, empty;

	for (; i >= 0; --i)
	{
		for (j = 0, empty = 0; j < symCount; ++j)
			if (data[i] != symbols[j])
				++empty;
		if (empty == symCount)
			break;
	}

	return data.substr(0, i + 1);
}

//-------------------------------------------------------------------

dodoString
string::lTrim(const dodoString &data,
	      const char symbols[],
	      int symCount)
{
	int size = data.size(), i(0), empty, j;

	for (; i < size; ++i)
	{
		for (j = 0, empty = 0; j < symCount; ++j)
			if (data[i] != symbols[j])
				++empty;
		if (empty == symCount)
			break;
	}

	return data.substr(i, size - i);
}

//-------------------------------------------------------------------

dodoString
string::trim(const dodoString &data,
	     const char symbols[],
	     int symCount)
{
	return rTrim(lTrim(data, symbols, symCount), symbols, symCount);
}

//-------------------------------------------------------------------

dodoString
string::trim(const dodoString &data,
	     char symbol)
{
	return rTrim(lTrim(data, symbol), symbol);
}

//-------------------------------------------------------------------

long
string::stringToL(const dodoString &data)
{
	return atol(data.c_str());
}

//-------------------------------------------------------------------

unsigned long
string::stringToUL(const dodoString &data)
{
	return strtoul(data.c_str(), NULL, 10);
}

//-------------------------------------------------------------------

int
string::stringToI(const dodoString &data)
{
	return atoi(data.c_str());
}

//-------------------------------------------------------------------

unsigned int
string::stringToUI(const dodoString &data)
{
	return (unsigned int)atol(data.c_str());
}

//-------------------------------------------------------------------

short
string::stringToS(const dodoString &data)
{
	return (short)atoi(data.c_str());
}

//-------------------------------------------------------------------

unsigned short
string::stringToUS(const dodoString &data)
{
	return (unsigned short)atoi(data.c_str());
}

//-------------------------------------------------------------------

float
string::stringToF(const dodoString &data)
{
	return (float)atof(data.c_str());
}

//-------------------------------------------------------------------

double
string::stringToD(const dodoString &data)
{
	return atof(data.c_str());
}

//-------------------------------------------------------------------

void
string::replace(const dodoStringArray &needle,
		const dodoStringArray &replacement,
		dodoString &data)
{
	dodoStringArray::const_iterator i(needle.begin()), j(needle.end());
	dodoStringArray::const_iterator o(replacement.begin()), p(replacement.end());
	for (; i != j && o != p; ++i, ++o)
		replace(*i, *o, data);
}

//-------------------------------------------------------------------

void
string::replace(const dodoString &needle,
		const dodoString &replacement,
		dodoString &data)
{
	unsigned long i(0), j(needle.size()), k(replacement.size());

	while (true)
	{
		i = data.find(needle, i);
		if (i == dodoString::npos)
			break;

		data.replace(i, j, replacement, 0, k);
		i += k;
	}
}

//-------------------------------------------------------------------

