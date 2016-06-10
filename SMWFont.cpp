#include "stdafx.h"

#include "SMWFont.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "HUD.h"

void SMWFont::PaintPhrase(std::string phrase, int left, int top, int attributeFlags)
{
	left += CHARACTER_WIDTH / 2;
	int xo = left;
	int yo = top + CHARACTER_HEIGHT / 2;

	int srcX;
	int srcY;
	for (size_t i = 0; i < phrase.length(); ++i)
	{
		char currentChar = phrase.at(i);
		int spriteIndex = 0;
		if (currentChar == '_') spriteIndex = 66;
		else if (currentChar >= '0' && currentChar <= '9')
		{
			std::string numberStr; // Paint several numbers at once
			while (i < phrase.length() && currentChar >= '0' && currentChar <= '9')
			{
				numberStr += std::to_string(int(currentChar - '0'));
				++i;
				if (i < phrase.length()) currentChar = phrase.at(i);
			}
			--i;
			xo += (numberStr.length()-1) * CHARACTER_WIDTH;

			HUD::PaintSeveralDigitNumber(xo - 4, top + 2, stoi(numberStr), false);
			xo += CHARACTER_WIDTH;
			continue;
		}
		else if (currentChar >= 'A' && currentChar <= 'z')
		{
			if (currentChar <= 'Z') spriteIndex = (currentChar - 'A'); // Uppercase
			else spriteIndex = (currentChar - 'a' + ('Z' - 'A') + 1);  // Lowercase
		}
		else if (currentChar == '\n')
		{
			xo = left;
			yo += CHARACTER_HEIGHT - 1;
			continue;
		}
		else if (currentChar == ' ')
		{
			xo += CHARACTER_WIDTH;
			continue;
		}
		else if (currentChar == '?') spriteIndex = 54;
		else if (currentChar == '.') spriteIndex = 56;
		else if (currentChar == ',') spriteIndex = 57;
		else if (currentChar == '(') spriteIndex = 58;
		else if (currentChar == ')') spriteIndex = 59;
		else if (currentChar == '#') spriteIndex = 60;
		else if (currentChar == '@') spriteIndex = 62; // NOTE: @ refers to the clock symbol
		else if (currentChar == '*') spriteIndex = 63; // NOTE: * refers to multiplication X
		else if (currentChar == '=') spriteIndex = 64;
		else if (currentChar == '!') spriteIndex = 65;
		else if (currentChar == '\'') spriteIndex = 67;

		srcX = spriteIndex % CHARS_WIDE;
		srcY = int(spriteIndex / CHARS_WIDE);

		if (attributeFlags & OUTLINED) srcY += 4;

		SpriteSheet* spriteSheetPtr = SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::FONT);
		if (attributeFlags & INVERTED) spriteSheetPtr = SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::FONT_INVERTED);
		
		spriteSheetPtr->Paint(xo, yo, srcX, srcY);

		xo += CHARACTER_WIDTH;
	}
}
