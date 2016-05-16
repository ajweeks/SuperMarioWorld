#pragma once

#include "Item.h"
#include "CountdownTimer.h"

class Block : public Item
{
public:
	Block(DOUBLE2 topLeft, Item::TYPE type, Level* levelPtr, BodyType bodyType = BodyType::STATIC);
	virtual ~Block();

	Block(const Block&) = delete;
	Block& operator=(const Block&) = delete;

	virtual void Tick(double deltaTime) = 0;
	virtual void Paint() = 0;
	virtual void Hit() = 0;

	static const int WIDTH = 16;
	static const int HEIGHT = 16;
};