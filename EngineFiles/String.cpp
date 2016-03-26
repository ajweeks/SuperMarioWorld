//-----------------------------------------------------------------
// Game Engine
// C++ Source - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------
#include "stdafx.h"
#include "../stdafx.h"

#include "String.h"

using namespace std;

//-----------------------------------------------------------------
// global String operator overloads
//-----------------------------------------------------------------

String& operator+=(String& lhs, const char* singleTextPtr)
{
	return lhs += String(singleTextPtr);
}

String operator+(String & lhs, char* singleTextPtr)
{
	String newString = lhs;
	newString += singleTextPtr;
	return newString;
}

//-----------------------------------------------------------------
// String methods
//-----------------------------------------------------------------

String::String(const String& sRef)
{
	m_Length = sRef.Length() + 1; // include room for null terminator
	m_TextPtr = new char[m_Length];

	strcpy_s(m_TextPtr, m_Length, sRef.C_str());
}

String::String(const char* singleTextPtr)
{
	m_Length = (int)strlen(singleTextPtr) + 1; // include room for null terminator

	m_TextPtr = new char[m_Length];

	strcpy_s((char*)m_TextPtr, m_Length, singleTextPtr);
}

String::String(char character)
{
	m_Length = 2; // include room for null terminator
	m_TextPtr = new char[m_Length];

	m_TextPtr[0] = character;
	m_TextPtr[1] = '\0';
}

String::String(int number)
{
	m_Length = 1; // include room for null terminator
	m_TextPtr = new char[1];
	m_TextPtr[0] = '\0';
	*this += to_string(number).c_str();
}

String::String(unsigned int number)
{
	m_Length = 1; // include room for null terminator
	m_TextPtr = new char[1];
	m_TextPtr[0] = '\0';
	*this += to_string(number).c_str();
}

String::String(double number, int precision)
{
	m_Length = 1; // include room for null terminator
	m_TextPtr = new char[1];
	m_TextPtr[0] = '\0';
	*this += to_string(number).c_str();

	// http://www.cplusplus.com/reference/ios/ios_base/precision/

	stringstream buffer;
	if (precision > -1) buffer << std::fixed << std::setprecision(precision) << number;
	*this = String(buffer.str().c_str());
}

String::~String()
{
	delete m_TextPtr;
}

String& String::operator=(const String& sRef)
{
	if (this != &sRef) // beware of self assignment: s = s
	{
		delete m_TextPtr;
		m_Length = sRef.Length() + 1;
		m_TextPtr = new char[m_Length];
		strcpy_s(m_TextPtr, m_Length, sRef.C_str());
	}
	return *this;
}

String& String::operator+=(const String& sRef)
{
	m_Length = this->Length() + sRef.Length() + 1;

	char* buffer = new char[m_Length];

	strcpy_s(buffer, m_Length, m_TextPtr);
	strcat_s(buffer, m_Length, sRef.m_TextPtr);

	delete m_TextPtr;
	m_TextPtr = buffer;

	return *this;
}

String String::operator+(String const& sRef) const
{
	String newString = *this;

	newString += sRef;

	return newString;
}

bool String::operator==(String const& sRef) const
{
	return this->Compare(sRef);
}

bool String::operator!=(String const& sRef) const
{
	return !this->Compare(sRef);
}

char String::At(int index) const
{
	if (index > this->Length() - 1) return 0;

	return m_TextPtr[index];
}

String String::SubStr(int index) const
{
	if (index > this->Length() - 1) return String("");

	return String(m_TextPtr + index);
}

String String::SubStr(int index, int length) const
{
	if (index + length - 1 > this->Length() - 1) return String("");

	String newString = *this;
	newString.m_TextPtr[index + length] = TEXT('\0');

	return String(newString.m_TextPtr + index);
}

int String::Find(char character) const
{
	for (int index = 0; index < m_Length - 1; ++index)
	{
		if (m_TextPtr[index] == character) return index;
	}
	return -1;
}

int String::Rfind(char character) const
{
	// start on pos, one before the \0
	for (int index = Length() - 1; index >= 0; --index)
	{
		if (m_TextPtr[index] == character) return index;
	}
	return -1;
}

int String::Find(const String & sRef) const
{
	// return false if 2nd string is longer than 1st string 
	if (this->Length() < sRef.Length()) return -1;

	//empty string(s)
	if (sRef.Length() < 1 || this->Length() < 1) return -1;
	int i = 0;
	do
	{
		String temp = this->SubStr(i, sRef.Length());
		if (sRef.Compare(temp)) return i;
		++i;
	} while (i <= this->Length() - sRef.Length());

	return -1;
}

int String::Rfind(const String& sRef) const
{
	// return false if 2nd string is longer than 1st string 
	if (this->Length() < sRef.Length()) return -1;

	//empty string(s)
	if (sRef.Length() < 1 || this->Length() < 1) return -1;
	int i = 0;
	do
	{
		String temp = this->SubStr(this->Length() - i - sRef.Length(), sRef.Length());
		if (sRef.Compare(temp)) return this->Length() - i - sRef.Length();
		++i;
	} while (i <= this->Length());

	return -1;
}

int String::Length() const
{
	return m_Length - 1; // don't include the null terminator when asked how many characters are in the string
}

bool String::Compare(String const& sRef) const
{
	if (sRef.Length() != this->Length()) return false;

	return strcmp(this->C_str(), sRef.C_str()) ? false : true; // return true if cmp returns 0, false if not
}

int String::ToInteger() const
{
	return atoi(this->C_str());
}

double String::ToDouble() const
{
	return strtod(this->C_str(), 0);
}

char* String::C_str() const
{
	return m_TextPtr;
}

