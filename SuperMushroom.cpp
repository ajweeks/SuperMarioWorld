#include "stdafx.h"

#include "SuperMushroom.h"

SuperMushroom::SuperMushroom(DOUBLE2 topLeft, Level* levelPtr, int directionFacing) :
	MoveableItem(topLeft, TYPE::SUPER_MUSHROOM, 2, 12, levelPtr, directionFacing)
{
}
