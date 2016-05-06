#include "stdafx.h"

#include "SMWFont.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "HUD.h"

const char SMWFont::m_LetterIndicies[] = { 'A', 'B', 'C' };

void SMWFont::PaintPhrase(std::string phrase, int left, int top, bool outlined)
{
	int xo = left;
	
	RECT2 srcRect;
	for (size_t i = 0; i < phrase.length(); ++i)
	{
		char currentChar = phrase.at(i);
		if (currentChar >= '0' && currentChar <= '9')
		{
			HUD::PaintSeveralDigitNumber(xo - 3, top + 2,  currentChar - '0', i > 0);
		}
		else if (currentChar >= 'A' && currentChar <= 'z')
		{
			int index;
			if (currentChar <= 'Z')
				index = (currentChar - 'A');
			else
				index = (currentChar - 'a' + ('Z' - 'A') + 1);

			srcRect.left = (index % CHARS_WIDE) * CHARACTER_WIDTH;
			srcRect.top = int(index / CHARS_WIDE) * CHARACTER_HEIGHT;
			srcRect.right = srcRect.left + CHARACTER_WIDTH;
			srcRect.bottom = srcRect.top + CHARACTER_HEIGHT;
		}
		else if (currentChar == '.')
		{
			int index = 61;
			srcRect.left = (index % CHARS_WIDE) * CHARACTER_WIDTH;
			srcRect.top = int(index / CHARS_WIDE) * CHARACTER_HEIGHT;
			srcRect.right = srcRect.left + CHARACTER_WIDTH;
			srcRect.bottom = srcRect.top + CHARACTER_HEIGHT;
		}
		else if (currentChar == ' ')
		{
			xo += CHARACTER_WIDTH;
			continue;
		}
		else
		{
			int x = 1;
			x++;
		}

		if (outlined)
		{
			srcRect.top += 36;
			srcRect.bottom += 36;
		}
		
		GAME_ENGINE->DrawBitmap(SpriteSheetManager::fontPtr, xo, top, srcRect);

		xo += CHARACTER_WIDTH;
	}
}
