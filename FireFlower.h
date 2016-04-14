#pragma once

#include "Item.h"

class FireFlower : public Item
{
public:
	FireFlower(DOUBLE2 topLeft, Level* levelPtr);
	void Tick(double deltaTime);
	void Paint();

};