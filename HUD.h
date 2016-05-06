#pragma once

class HUD
{
public:
	// NOTE: The x coordinate specifies the placement of the right-most digit
	static void PaintSeveralDigitNumber(int x, int y, int number, bool yellow = false);
	static void PaintSeveralDigitLargeNumber(int x, int y, int number);

	// NOTE: If yellow is true, this returns the rect for a yellow number, otherwise for a white number
	static RECT2 GetSmallSingleNumberSrcRect(int number, bool yellow = false);
	static RECT2 GetLargeSingleNumberSrcRect(int number);

private:
	HUD() = delete;
};