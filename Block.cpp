#include "stdafx.h"

#include "Block.h"

Block::Block(DOUBLE2 topLeft, TYPE type, Level* levelPtr) :
	Item(topLeft, type, levelPtr, BodyType::STATIC, WIDTH, HEIGHT)
{
}

Block::~Block() {}