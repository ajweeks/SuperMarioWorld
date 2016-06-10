#pragma once

class SMWFont
{
public:
	static const int OUTLINED = (1 << 1);
	static const int INVERTED = (1 << 2);

	static void PaintPhrase(std::string phrase, int left, int top, int attributeFlags);

	static const int CHARACTER_WIDTH = 8;
	static const int CHARACTER_HEIGHT = 9;
private:

	static const int CHARS_WIDE = 18;
};