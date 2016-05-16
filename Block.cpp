#include "stdafx.h"

#include "Block.h"

Block::Block(DOUBLE2 topLeft, TYPE type, Level* levelPtr, BodyType bodyType) :
	Item(topLeft, type, levelPtr, bodyType, WIDTH, HEIGHT)
{
}

Block::~Block() {}