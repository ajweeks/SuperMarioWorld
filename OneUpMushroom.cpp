#include "stdafx.h"

#include "OneUpMushroom.h"
#include "SpriteSheet.h"

OneUpMushroom::OneUpMushroom(DOUBLE2 topLeft, Level* levelPtr) :
	Item(topLeft, TYPE::ONE_UP_MUSHROOM, levelPtr)
{
}

void OneUpMushroom::Tick(double deltaTime)
{
}

void OneUpMushroom::Paint()
{
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	double srcCol = 2;
	double srcRow = 12;

	m_SpriteSheetPtr->Paint(left, top, srcCol, srcRow);
}
