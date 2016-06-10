#include "stdafx.h"

#include "SuperMushroom.h"

SuperMushroom::SuperMushroom(DOUBLE2 topLeft, Level* levelPtr, int directionFacing) :
	MoveableItem(topLeft, Type::SUPER_MUSHROOM, 2, 14, levelPtr, directionFacing)
{
}
