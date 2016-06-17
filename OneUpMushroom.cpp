#include "stdafx.h"

#include "OneUpMushroom.h"

OneUpMushroom::OneUpMushroom(DOUBLE2 topLeft, Level* levelPtr, int directionFacing, bool spawnedFromBlock) :
	MoveableItem(topLeft, Type::ONE_UP_MUSHROOM, 3, 14, levelPtr, directionFacing, spawnedFromBlock)
{
}
