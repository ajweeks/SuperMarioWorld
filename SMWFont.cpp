#include "stdafx.h"

#include "SMWFont.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "HUD.h"

void SMWFont::PaintPhrase(std::string phrase, int left, int top, bool outlined)
{
	int xo = left;
	
	RECT2 srcRect;
	for (size_t i = 0; i < phrase.length(); ++i)
	{
		char currentChar = phrase.at(i);
		int spriteIndex = 0;
		if (currentChar >= '0' && currentChar <= '9')
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

			HUD::PaintSeveralDigitNumber(xo, top + 2, stoi(numberStr), false);
			xo += CHARACTER_WIDTH;
			continue;
		}
		else if (currentChar >= 'A' && currentChar <= 'z')
		{
			if (currentChar <= 'Z')
				spriteIndex = (currentChar - 'A');
			else
				spriteIndex = (currentChar - 'a' + ('Z' - 'A') + 1);
		}
		else if (currentChar == '.')
		{
			spriteIndex = 61;
		}
		else if (currentChar == '@') // NOTE: @ refers to the clock symbol
		{
			spriteIndex = 62;
		}
		else if (currentChar == '*') // NOTE: * refers to multiplication X
		{
			spriteIndex = 63;
		}
		else if (currentChar == '=')
		{
			spriteIndex = 64;
		}
		else if (currentChar == '!')
		{
			spriteIndex = 65;
		}
		else if (currentChar == ' ')
		{
			xo += CHARACTER_WIDTH;
			continue;
		}

		srcRect.left = (spriteIndex % CHARS_WIDE) * CHARACTER_WIDTH;
		srcRect.top = int(spriteIndex / CHARS_WIDE) * CHARACTER_HEIGHT;
		srcRect.right = srcRect.left + CHARACTER_WIDTH;
		srcRect.bottom = srcRect.top + CHARACTER_HEIGHT;

		if (outlined)
		{
			srcRect.top += 36;
			srcRect.bottom += 36;
		}
		
		GAME_ENGINE->DrawBitmap(SpriteSheetManager::fontPtr, xo, top, srcRect);

		xo += CHARACTER_WIDTH;
	}
}
