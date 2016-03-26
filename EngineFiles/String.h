//-----------------------------------------------------------------
// Game Engine
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// String Class
//-----------------------------------------------------------------
class String
{
public:
	// -------------------------
	// Constructors & Destructor
	// -------------------------	
	//! Converts the first argument to a string 
	String(const String& sRef);
	//! Converts the first argument to a string 
	//explicit String(const wchar_t* wideTextPtr = L"");
	//! Converts the first argument to a string 
	explicit String(const char* singleTextPtr = "");
	//! Converts the first argument to a string 
	//explicit String(wchar_t character);
	//! Converts the first argument to a string 
	explicit String(char character);
	//! Converts the first argument to a string 
	explicit String(int number);
	//! Converts the first argument to a string 
	explicit String(unsigned int number);
	//! Converts the first argument to a string using precision digits 
	explicit String(double number, int precision = 2);

	virtual ~String();

	// -------------------------
	// General String Methods
	// -------------------------
	//! Returns a char that is a copy of the character at position index in this String.
	char At(int index) const;

	//! Returns a newly constructed String object with its value initialized to a copy of a substring of this object.
	//! The substring is the portion of the object that starts at character position index and spans until the end of the string).
	String SubStr(int index) const;

	//! Returns a newly constructed String object with its value initialized to a copy of a substring of this object.
	//! The substring is the portion of the object that starts at character position index and spans length characters(or until the end of the string, whichever comes first).
	String SubStr(int index, int length) const;

	//! Searches the String for the first occurrence of the character.
	//! @return the index of the first occurrence of the character if found or -1 if the character is not found
	int Find(char character) const;

	//! Searches the String for the first occurrence of the argument.
	//! @return the index of the first occurrence of the String argument if found or -1 if the String argument is not found
	int Find(const String& sRef) const;

	//! Searches the String for the last occurrence of the character.
	//! @return the index of the last occurrence of the character if found or -1 if the character is not found
	int Rfind(char character) const;

	//! Searches the String for the last occurrence of the argument.
	//! @return the index of the last occurrence of the String argument if found or -1 if the String argument is not found
	int Rfind(const String& sRef) const;

	//! @returns the length of the String, in terms of number of characters
	int Length() const;

	//! Compares the value of the String object to the value of the argument.
	bool Compare(const String& sRef) const;

	// -------------------------
	// Conversion Methods
	// -------------------------

	//! Returns a pointer to an array that contains a null - terminated sequence of characters(i.e., a C - string) representing the current value of the String object.
	char* C_str() const;

	//! Converts a string to a signed integer
	int ToInteger() const;

	//! Converts a string to a double
	double ToDouble() const;

	// ----------------------------------------
	// Overloaded operators: = , +=, +, and ==
	// ----------------------------------------

	//! Assigns a new value to the String, replacing its current contents.
	String& operator=(const String& sRef);

	//! Extends the String by appending the argument at the end of its current value
	String& operator+=(const String& sRef);

	//! Returns a String that is the concatenation of this String and the argument
	String operator+(const String& sRef) const;

	//! Compares this string and the argument
	bool operator==(const String& sRef) const;

	//! Compares this string and the argument
	bool operator!=(const String& sRef) const;

private:
	//! converts the String to a wchar
	wchar_t* String::ToWchar() const;

	// -------------------------
	// Datamembers
	// -------------------------
	char* m_TextPtr;
	int m_Length;
};
