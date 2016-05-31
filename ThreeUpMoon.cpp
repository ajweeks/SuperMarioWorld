#include "stdafx.h"

#include "ThreeUpMoon.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

ThreeUpMoon::ThreeUpMoon(DOUBLE2 topLeft, Level* levelPtr) :
	Item(topLeft, Type::THREE_UP_MOON, levelPtr)
{
}

void ThreeUpMoon::Tick(double deltaTime)
{
	Item::Tick(deltaTime);
}

void ThreeUpMoon::Paint()
{
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	int srcCol = 3;
	int srcRow = 7;

	SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES)->Paint(left, top, srcCol, srcRow);
}
