#pragma once

#include "Item.h"

class OneUpMushroom : public Item
{
public:
	OneUpMushroom(DOUBLE2 topLeft, Level* levelPtr);

	OneUpMushroom(const OneUpMushroom&) = delete;
	OneUpMushroom& operator=(const OneUpMushroom&) = delete;

	void Tick(double deltaTime);
	void Paint();

};