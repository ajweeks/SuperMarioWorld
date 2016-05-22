
#pragma once

#include "Item.h"

class ThreeUpMoon : public Item
{
public:
	ThreeUpMoon(DOUBLE2 topLeft, Level* levelPtr);

	ThreeUpMoon(const ThreeUpMoon&) = delete;
	ThreeUpMoon& operator=(const ThreeUpMoon&) = delete;

	void Tick(double deltaTime);
	void Paint();

};