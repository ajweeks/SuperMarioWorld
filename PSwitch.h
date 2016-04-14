#pragma once

#include "Item.h"

class PSwitch : public Item
{
public:
	PSwitch(DOUBLE2 topLeft, COLOUR colour, Level* levelPtr);

	void Tick(double deltaTime);
	void Paint();

private:
	COLOUR m_Colour;
	bool m_IsPressed = false;

};
