#pragma once

class FileIO
{
public:
	static std::string GetTagContent(std::string totalString, std::string tagString, int startPos = 0);

	static RECT2 StringToRECT2(std::string string);
	static bool StringToBool(std::string valueString);
	static std::string BoolToString(bool value);

private:
	FileIO() = delete;
};