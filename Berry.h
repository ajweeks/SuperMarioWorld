#pragma once

#include "Item.h"
#include "Enumerations.h"

class Berry : public Item
{
public:
	Berry(DOUBLE2& positionRef, Level* levelPtr, COLOUR colour);
	virtual ~Berry();

	void Tick(double deltaTime);
	void Paint();

private:
	COLOUR m_Colour;

};

