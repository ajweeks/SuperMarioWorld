#pragma once

#include "Item.h"

class FireFlower : public Item
{
public:
	FireFlower(DOUBLE2 topLeft, Level* levelPtr);

	FireFlower(const FireFlower&) = delete;
	FireFlower& operator=(const FireFlower&) = delete;

	void Tick(double deltaTime);
	void Paint();
};