#pragma once

#include "Item.h"

class CapeFeather : public Item
{
public:
	CapeFeather(DOUBLE2 topLeft, Level* levelPtr);
	void Tick(double deltaTime);
	void Paint();

};