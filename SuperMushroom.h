#pragma once

#include "MoveableItem.h"

class SuperMushroom : public MoveableItem
{
public:
	SuperMushroom(DOUBLE2 topLeft, Level* levelPtr, int directionFacing = Direction::RIGHT);

	SuperMushroom(const SuperMushroom&) = delete;
	SuperMushroom& operator=(const SuperMushroom&) = delete;

};