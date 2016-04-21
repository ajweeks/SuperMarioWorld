#include "stdafx.h"

#include "OneUpMushroom.h"
#include "SpriteSheetManager.h"
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
	int srcCol = 2;
	int srcRow = 12;

	SpriteSheetManager::generalTilesPtr->Paint(left, top, srcCol, srcRow);
}
