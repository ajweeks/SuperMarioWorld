#pragma once

#include "INT2.h"

class FileIO
{
public:
	static std::string GetTagContent(const std::string& totalString, const std::string& tagString, int startPos = 0);

	static RECT2 StringToRECT2(const std::string& string);
	static bool StringToBool(const std::string& valueString);
	static std::string BoolToString(bool value);
	static INT2 StringToINT2(const std::string& int2String);

private:
	FileIO() = delete;
};

// Removes whitespace from level data read from file, but preserves spacing in quotes
class IsWhitespace
{
public:
	IsWhitespace() {}

	bool operator()(int c)
	{
		if (m_InQuote)
		{
			if (c == '\"')
			{
				m_InQuote = false;
				return true;
			}
			else if (c == '\t')
			{
				return true;
			}
			return false;
		}
		else if (c == '\"')
		{
			m_InQuote = true;
			return true;
		}
		else
		{
			return !!isspace(c);
		}
	}

private:
	bool m_InQuote = false;

};