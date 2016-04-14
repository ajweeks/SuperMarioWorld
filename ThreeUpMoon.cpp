#include "stdafx.h"

#include "ThreeUpMoon.h"
#include "SpriteSheet.h"

ThreeUpMoon::ThreeUpMoon(DOUBLE2 topLeft, Level* levelPtr) :
	Item(topLeft, TYPE::THREE_UP_MOON, levelPtr)
{
}

void ThreeUpMoon::Tick(double deltaTime)
{
}

void ThreeUpMoon::Paint()
{
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	double srcCol = 3;
	double srcRow = 7;

	m_SpriteSheetPtr->Paint(left, top, srcCol, srcRow);
}
