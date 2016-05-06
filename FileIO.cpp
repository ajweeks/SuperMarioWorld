#include "stdafx.h"

#include "FileIO.h"

std::string FileIO::GetTagContent(std::string totalString, std::string tagString, int startPos)
{
	std::string result;

	int attributeBegin = totalString.find("<" + tagString + ">", startPos);
	if (attributeBegin != -1)
	{
		attributeBegin += std::string("<" + tagString + ">").length();
	}
	int attributeEnd = totalString.find("</" + tagString + ">", attributeBegin);
	if (attributeBegin == std::string::npos || attributeEnd == std::string::npos)
	{
		return "";
	}
	result = totalString.substr(attributeBegin, attributeEnd - attributeBegin);

	return result;
}

RECT2 FileIO::StringToRECT2(std::string string)
{
	RECT2 result;

	int comma1 = string.find(",");
	int comma2 = string.find(",", comma1 + 1);
	int comma3 = string.find(",", comma2 + 1);

	result.left = stod(string.substr(0, comma1));
	result.top = stod(string.substr(comma1 + 1, comma2 - comma1 - 1));
	result.right = stod(string.substr(comma2 + 1, comma3 - comma2 - 1));
	result.bottom = stod(string.substr(comma3 + 1));

	return result;
}

bool FileIO::StringToBool(std::string valueString)
{
	return valueString != "0";
}

std::string FileIO::BoolToString(bool value)
{
	return (value == 1 ? "true" : value == 0 ? "false" : "");
}