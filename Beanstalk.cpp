#include "stdafx.h"

#include "Beanstalk.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "Level.h"
#include "PrizeBlock.h"

Beanstalk::Beanstalk(DOUBLE2 bottomLeft, Level* levelPtr, int height) : 
	Item(bottomLeft + DOUBLE2(TILE_SIZE/2 - VINE_WIDTH/2, -height * TILE_SIZE), 
		Item::Type::BEANSTALK, levelPtr, Level::BEANSTALK, BodyType::STATIC, VINE_WIDTH, height * TILE_SIZE),
	FINAL_HEIGHT(height * TILE_SIZE)
{
	m_CurrentHeight = 0;
	m_ActPtr->SetSensor(true);
	m_IsActive = true;
}

Beanstalk::~Beanstalk()
{
}

void Beanstalk::Tick(double deltaTime)
{
	if (m_CurrentHeight < FINAL_HEIGHT)
	{
		m_CurrentHeight += 1;

		m_AnimInfo.Tick(deltaTime);
		m_AnimInfo.frameNumber %= 2;

		if (m_CurrentHeight >= FINAL_HEIGHT)
		{
			m_CurrentHeight = FINAL_HEIGHT;
			m_AnimInfo.frameNumber = 0;

			DOUBLE2 topLeft(m_ActPtr->GetPosition() - DOUBLE2(TILE_SIZE / 2, HEIGHT / 2 + TILE_SIZE / 2));
			PrizeBlock* prizeBlockPtr = new PrizeBlock(topLeft, m_LevelPtr);
			prizeBlockPtr->SetUsed(true);
			m_LevelPtr->AddItem(prizeBlockPtr);
		}
	}
}

void Beanstalk::Paint()
{
	double centerX = m_ActPtr->GetPosition().x;
	double centerY = m_ActPtr->GetPosition().y + HEIGHT / 2.0;

	for (int i = 0; i < m_CurrentHeight / TILE_SIZE; ++i)
	{
		SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::BEANSTALK)->Paint(centerX, centerY, 0, 2);
		centerY -= 16;
	}
	if (m_CurrentHeight < FINAL_HEIGHT)
	{
		SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::BEANSTALK)->Paint(centerX, centerY, 0, 0 + m_AnimInfo.frameNumber);
	}
	else
	{
		SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES)->Paint(centerX, centerY, 4, 4);
	}
}
