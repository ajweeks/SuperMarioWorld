#include "stdafx.h"

#include "CloudBlock.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

CloudBlock::CloudBlock(DOUBLE2 topLeft, Level* levelPtr) :
	Block(topLeft, Item::Type::CLOUD_BLOCK, levelPtr)
{
}

CloudBlock::~CloudBlock()
{
}

void CloudBlock::Tick(double deltaTime)
{
}

void CloudBlock::Paint()
{
	double centerX = m_ActPtr->GetPosition().x;
	double centerY = m_ActPtr->GetPosition().y;
	SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES)->Paint(centerX, centerY, 3, 11);
}

void CloudBlock::Hit()
{
}
