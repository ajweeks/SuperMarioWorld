#pragma once

#include "Item.h"
#include "SMWTimer.h"

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
	static const int WIDTH = 10;
	static const int HEIGHT = 14;
	bool m_IsPressed = false;

	// This timer starts when we get pressed by the player and we are removed when it finishes
	SMWTimer m_PressedTimer; 

};
