#pragma once

class SMWFont
{
public:
	static void PaintPhrase(std::string phrase, int left, int top, bool outlined);

private:
	static const int CHARACTER_WIDTH = 8;
	static const int CHARACTER_HEIGHT = 9;

	static const int CHARS_WIDE = 18;

	static const char m_LetterIndicies[];
};