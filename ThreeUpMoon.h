#pragma once

#include "Item.h"

class ThreeUpMoon : public Item
{
public:
	ThreeUpMoon(DOUBLE2 topLeft, Level* levelPtr);
	void Tick(double deltaTime);
	void Paint();

};