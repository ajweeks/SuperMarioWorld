#include "stdafx.h"

#include "ThreeUpMoon.h"
#include "SpriteSheetManager.h"
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
	int srcCol = 3;
	int srcRow = 7;

	SpriteSheetManager::generalTilesPtr->Paint(left, top, srcCol, srcRow);
}
