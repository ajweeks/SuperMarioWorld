#pragma once

#include "Item.h"

class OneUpMushroom : public Item
{
public:
	OneUpMushroom(DOUBLE2 topLeft, Level* levelPtr);
	void Tick(double deltaTime);
	void Paint();

};