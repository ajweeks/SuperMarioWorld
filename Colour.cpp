#include "stdafx.h"

#include "Colour.h"

std::string Colour::COLOURToString(COLOUR colour)
{
	switch (colour)
	{
	case COLOUR::RED:
		return "Red";
	case COLOUR::GREEN:
		return "Green";
	case COLOUR::YELLOW:
		return "Yellow";
	case COLOUR::BLUE:
		return "Blue";
	case COLOUR::ORANGE:
		return "Orange";
	case COLOUR::PINK:
		return "Pink";
	case COLOUR::GREY:
		return "Grey";
	default:
		return "unknown colour";
	}
}

COLOUR Colour::StringToCOLOUR(std::string str)
{
	if (!str.compare("Red")) return COLOUR::RED;
	else if (!str.compare("Green")) return COLOUR::GREEN;
	else if (!str.compare("Yellow")) return COLOUR::YELLOW;
	else if (!str.compare("Blue")) return COLOUR::BLUE;
	else if (!str.compare("Orange")) return COLOUR::ORANGE;
	else if (!str.compare("Pink")) return COLOUR::PINK;
	else if (!str.compare("Grey")) return COLOUR::GREY;
	else return COLOUR::NONE;
}
