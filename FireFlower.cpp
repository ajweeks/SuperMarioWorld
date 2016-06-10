#include "stdafx.h"

#include "FireFlower.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

FireFlower::FireFlower(DOUBLE2 topLeft, Level* levelPtr) :
	Item(topLeft, Type::FIRE_FLOWER, levelPtr)
{
}

void FireFlower::Tick(double deltaTime)
{
	Item::Tick(deltaTime);
	if (m_IsActive == false)
	{
		m_LevelPtr->RemoveItem(this);
		return;
	}
}

void FireFlower::Paint()
{
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	int srcCol = 4 + m_AnimInfo.frameNumber;
	int srcRow = 16;

	SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES)->Paint(left, top, srcCol, srcRow);
}
