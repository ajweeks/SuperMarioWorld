#pragma once

#include "Enumerations.h"

class SMWColour
{
public:
	static std::string COLOURToString(Colour colour);

	static Colour StringToCOLOUR(std::string str);
};