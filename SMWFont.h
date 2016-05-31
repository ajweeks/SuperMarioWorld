#pragma once

class SMWFont
{
public:
	static void PaintPhrase(std::string phrase, int left, int top, bool outlined);

	static const int CHARACTER_WIDTH = 8;
	static const int CHARACTER_HEIGHT = 9;
private:

	static const int CHARS_WIDE = 18;
};