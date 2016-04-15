#include "stdafx.h"

#include "PrizeBlock.h"
#include "Coin.h"
#include "Level.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "SoundManager.h"

PrizeBlock::PrizeBlock(DOUBLE2 topLeft, Level* levelPtr) :
	Block(topLeft, TYPE::PRIZE_BLOCK, levelPtr)
{
}

void PrizeBlock::Tick(double deltaTime)
{
	if (m_ShouldSpawnCoin)
	{
		Coin* newCoinPtr = new Coin(DOUBLE2(m_ActPtr->GetPosition().x - WIDTH / 2, m_ActPtr->GetPosition().y - HEIGHT),
			m_LevelPtr,
			Coin::LIFETIME);
		m_LevelPtr->AddItem(newCoinPtr);

		m_ShouldSpawnCoin = false;
	}

	if (m_CurrentFrameOfBumpAnimation > -1)
	{
		// NOTE: I *think* what happens during the bump animation is
		//		 the block renders the normal question mark (not spinning)
		//       and moves up then down around 3 or 5 pixels.
		//		 Then the block is "used" and renders the brown used block texture

		m_yo = m_CurrentFrameOfBumpAnimation;
		if (m_yo > m_FramesOfBumpAnimation / 2)
		{
			m_yo = (m_FramesOfBumpAnimation / 2) - (m_yo - (m_FramesOfBumpAnimation / 2));
		}
		m_yo = int(m_yo * -0.5);

		m_CurrentFrameOfBumpAnimation++;
		if (m_CurrentFrameOfBumpAnimation > m_FramesOfBumpAnimation)
		{
			m_CurrentFrameOfBumpAnimation = -1;
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
	double srcCol = 0 + m_AnimInfo.frameNumber;
	double srcRow = 4;

	if (m_IsUsed)
	{
		srcCol = 4;
	}

	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y + m_yo * 3;
	SpriteSheetManager::generalTilesPtr->Paint(left, top, srcCol, srcRow);
}

void PrizeBlock::Hit(Level* levelPtr)
{
	SoundManager::PlaySoundEffect(SoundManager::SOUND::BLOCK_HIT);

	if (m_IsUsed == false)
	{
		m_IsUsed = true;
		m_CurrentFrameOfBumpAnimation = 2;
		m_yo = 0;

		m_ShouldSpawnCoin = true;
	}
}
