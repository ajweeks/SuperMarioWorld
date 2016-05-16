#pragma once

struct Constants
{
	static const int NUM_LEVELS = 2;
};

enum class ActorId
{
	PLAYER, PLATFORM, PIPE, ENEMY, LEVEL, ITEM, YOSHI, YOSHI_TOUNGE
};

struct Direction
{
	static const int LEFT = -1;
	static const int RIGHT = 1;

	static const int UP = -1;
	static const int DOWN = 1;
};

enum class COLOUR
{
	RED, GREEN, YELLOW, BLUE, ORANGE, PINK, GREY, NONE
};

class SMWMath
{
	static unsigned int NumberOfDigits(unsigned int i)
	{
		return i > 0 ? int(log10(double(i))) + 1 : 1;
	}
};
