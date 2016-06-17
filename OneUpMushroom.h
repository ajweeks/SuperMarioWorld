#pragma once

#include "MoveableItem.h"

class OneUpMushroom : public MoveableItem
{
public:
	OneUpMushroom(DOUBLE2 topLeft, Level* levelPtr, int directionFacing = Direction::RIGHT, bool spawnedFromBlock = true);

	OneUpMushroom(const OneUpMushroom&) = delete;
	OneUpMushroom& operator=(const OneUpMushroom&) = delete;
};
