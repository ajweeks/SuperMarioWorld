#include "stdafx.h"

#include "CloudBlock.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

CloudBlock::CloudBlock(DOUBLE2 topLeft, Level* levelPtr) :
	Block(topLeft, Item::TYPE::CLOUD_BLOCK, levelPtr)
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
	SpriteSheetManager::generalTilesPtr->Paint(centerX, centerY, 3, 11);
}

void CloudBlock::Hit()
{
}
