#include "stdafx.h"

#include "HUD.h"
#include "Game.h"
#include "SpriteSheetManager.h"

void HUD::PaintSeveralDigitNumber(int x, int y, int number, bool yellow)
{
	number = abs(number);

	do {
		int digit = number % 10;
		RECT2 srcRect = GetSmallSingleNumberSrcRect(digit, yellow);
		GAME_ENGINE->DrawBitmap(SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::HUD), x, y, srcRect);

		x -= 8;
		number /= 10;
	} while (number > 0);
}

RECT2 HUD::GetSmallSingleNumberSrcRect(int number, bool yellow)
{
	assert(number >= 0 && number <= 9);

	RECT2 result;

	int numberWidth = 8;
	int numberHeight = 7;
	int xo = 0 + numberWidth * number;
	int yo = 34;

	if (yellow) yo += 10;

	result = RECT2(xo, yo, xo + numberWidth, yo + numberHeight);

	return result;
}

void HUD::PaintSeveralDigitLargeNumber(int x, int y, int number)
{
	number = abs(number);

	do {
		int digit = number % 10;
		RECT2 srcRect = GetLargeSingleNumberSrcRect(digit);
		GAME_ENGINE->DrawBitmap(SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::HUD), x, y, srcRect);

		x -= 8;
		number /= 10;
	} while (number > 0);
}

RECT2 HUD::GetLargeSingleNumberSrcRect(int number)
{
	assert(number >= 0 && number <= 9);

	RECT2 result;

	int numberWidth = 8;
	int numberHeight = 14;
	int xo = 0 + numberWidth * number;
	int yo = 19;

	result = RECT2(xo, yo, xo + numberWidth, yo + numberHeight);

	return result;
}