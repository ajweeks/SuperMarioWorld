#include "stdafx.h"

#include "Block.h"
#include "Level.h"

Block::Block(DOUBLE2 topLeft, Type type, Level* levelPtr, BodyType bodyType) :
	Item(topLeft, type, levelPtr, Level::BLOCK, bodyType, WIDTH, HEIGHT)
{
	b2Filter collisionFilter = m_ActPtr->GetCollisionFilter();
	collisionFilter.maskBits |= Level::YOSHI | Level::ITEM | Level::SHELL;
	m_ActPtr->SetCollisionFilter(collisionFilter);
	m_IsActive = true;
}

Block::~Block() {}