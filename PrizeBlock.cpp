#include "stdafx.h"

#include "PrizeBlock.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "SoundManager.h"
#include "Level.h"
#include "Coin.h"
#include "Yoshi.h"
#include "OneUpMushroom.h"
#include "SuperMushroom.h"

PrizeBlock::PrizeBlock(DOUBLE2 topLeft, Level* levelPtr, std::string spawnTypeStr) :
	Block(topLeft, TYPE::PRIZE_BLOCK, levelPtr),
	m_SpawnTypeStr(spawnTypeStr)
{
	m_AnimInfo.secondsPerFrame = 0.09;
	m_BumpAnimationTimer = CountdownTimer(14);
}

void PrizeBlock::Tick(double deltaTime)
{
	if (m_ShouldSpawnItem)
	{
		m_ShouldSpawnItem = false;

		DOUBLE2 itemPos(m_ActPtr->GetPosition().x - WIDTH / 2, m_ActPtr->GetPosition().y - HEIGHT / 2);
		if (!m_SpawnTypeStr.compare("Yoshi"))
		{
			if (m_LevelPtr->IsYoshiAlive())
			{
				OneUpMushroom* oneUpMushroomPtr = new OneUpMushroom(itemPos + DOUBLE2(3, -2), m_LevelPtr);
				m_LevelPtr->AddItem(oneUpMushroomPtr, true);
			}
			else
			{
				Yoshi* newYoshiPtr = new Yoshi(itemPos - DOUBLE2(0, HEIGHT/2), m_LevelPtr);
				m_LevelPtr->AddYoshi(newYoshiPtr);
			}
		}
		else
		{
			Coin* newCoinPtr = new Coin(itemPos, m_LevelPtr, Coin::LIFETIME);
			m_LevelPtr->AddItem(newCoinPtr);
		}
	}

	if (m_BumpAnimationTimer.Tick())
	{
		// NOTE: I *think* what happens during the bump animation is
		//		 the block renders the normal question mark (not spinning)
		//       and moves up then down around 3 or 5 pixels.
		//		 Then the block is "used" and renders the brown used block texture

		m_yo = m_BumpAnimationTimer.FramesElapsed();
		if (m_yo > m_BumpAnimationTimer.OriginalNumberOfFrames() / 2)
		{
			m_yo = (m_BumpAnimationTimer.OriginalNumberOfFrames() / 2) - (m_yo - (m_BumpAnimationTimer.OriginalNumberOfFrames() / 2));
		}
		m_yo = int(m_yo * -0.5);

		if (m_BumpAnimationTimer.IsComplete())
		{
			m_yo = 0;
		}
	}
	else
	{
		m_AnimInfo.Tick(deltaTime);
		m_AnimInfo.frameNumber %= 4;
	}
}

void PrizeBlock::Paint()
{
	int srcCol = 0 + m_AnimInfo.frameNumber;
	int srcRow = 4;

	if (m_IsUsed)
	{
		srcCol = 4;
	}

	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y + m_yo * 3;
	SpriteSheetManager::generalTilesPtr->Paint(left, top, srcCol, srcRow);
}

void PrizeBlock::Hit()
{
	SoundManager::PlaySoundEffect(SoundManager::SOUND::BLOCK_HIT);

	if (m_IsUsed == false)
	{
		m_IsUsed = true;
		m_BumpAnimationTimer.Start();
		m_yo = 0;

		m_ShouldSpawnItem = true;
	}
}

void PrizeBlock::SetUsed(bool used)
{
	m_IsUsed = used;
}
