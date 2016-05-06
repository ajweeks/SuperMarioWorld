#pragma once

#include "Enumerations.h"

class Colour
{
public:
	static std::string COLOURToString(COLOUR colour);

	static COLOUR StringToCOLOUR(std::string str);
};