#pragma once

#include "Item.h"
#include "CountdownTimer.h"

class PSwitch : public Item
{
public:
	PSwitch(DOUBLE2 topLeft, Colour colour, Level* levelPtr);

	PSwitch(const PSwitch&) = delete;
	PSwitch& operator=(const PSwitch&) = delete;

	void Tick(double deltaTime);
	void Paint();
	void Hit();

private:
	Colour m_Colour;
	bool m_IsPressed = false;

	// This timer starts when we get pressed by the player and we are removed when it finishes
	CountdownTimer m_PressedTimer; 

};
