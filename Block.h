#pragma once

#include "Item.h"

class Block : public Item
{
public:
	static const int WIDTH = 16;
	static const int HEIGHT = 16;

	Block(DOUBLE2 topLeft, Item::TYPE type, Level* levelPtr);
	virtual ~Block();
	virtual void Tick(double deltaTime) = 0;
	virtual void Paint() = 0;
	virtual void Hit(Level* levelPtr) = 0;

};