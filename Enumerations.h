#pragma once

enum class ActorId
{
	PLAYER, PLATFORM, PIPE, ENEMY, LEVEL, ITEM, YOSHI, YOSHI_TOUNGE
};

struct FacingDirection
{
	static const int LEFT = -1;
	static const int RIGHT = 1;
};

enum class COLOUR
{
	RED, GREEN, YELLOW, BLUE, ORANGE, PINK, GREY, NONE
};

class SMWMath
{
	static unsigned int NumberOfDigits(unsigned int i)
	{
		return i > 0 ? (int)log10((double)i) + 1 : 1;
	}
};
