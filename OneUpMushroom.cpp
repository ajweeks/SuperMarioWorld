#include "stdafx.h"

#include "OneUpMushroom.h"

OneUpMushroom::OneUpMushroom(DOUBLE2 topLeft, Level* levelPtr, int directionFacing) :
	MoveableItem(topLeft, Type::ONE_UP_MUSHROOM, 3, 12, levelPtr, directionFacing)
{
}
