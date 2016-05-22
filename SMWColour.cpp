#include "stdafx.h"

#include "SMWColour.h"

std::string SMWColour::COLOURToString(Colour colour)
{
	switch (colour)
	{
	case Colour::RED:
		return "Red";
	case Colour::GREEN:
		return "Green";
	case Colour::YELLOW:
		return "Yellow";
	case Colour::BLUE:
		return "Blue";
	case Colour::ORANGE:
		return "Orange";
	case Colour::PINK:
		return "Pink";
	case Colour::GREY:
		return "Grey";
	default:
		return "unknown colour";
	}
}

Colour SMWColour::StringToCOLOUR(std::string str)
{
	if (!str.compare("Red")) return Colour::RED;
	else if (!str.compare("Green")) return Colour::GREEN;
	else if (!str.compare("Yellow")) return Colour::YELLOW;
	else if (!str.compare("Blue")) return Colour::BLUE;
	else if (!str.compare("Orange")) return Colour::ORANGE;
	else if (!str.compare("Pink")) return Colour::PINK;
	else if (!str.compare("Grey")) return Colour::GREY;
	else return Colour::NONE;
}
