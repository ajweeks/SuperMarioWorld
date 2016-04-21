#include "stdafx.h"

#include "ExclamationMarkBlock.h"
#include "SuperMushroom.h"
#include "Level.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "SoundManager.h"

ExclamationMarkBlock::ExclamationMarkBlock(DOUBLE2 topLeft, COLOUR colour, bool isSolid, Level* levelPtr):
	Block(topLeft, TYPE::EXCLAMATION_MARK_BLOCK, levelPtr), m_Colour(colour)
{
	m_AnimInfo.frameNumber = 0;
	SetSolid(isSolid);
}

void ExclamationMarkBlock::Tick(double deltaTime)
{
	if (m_ShouldSpawnSuperMushroom)
	{
		Item* newItem = new SuperMushroom(m_ActPtr->GetPosition() - DOUBLE2(SuperMushroom::WIDTH / 2, HEIGHT / 2 + 2), m_LevelPtr);
		newItem->AddContactListener(m_LevelPtr);
		m_LevelPtr->AddItem(newItem);

		m_ShouldSpawnSuperMushroom = false;
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
}

void ExclamationMarkBlock::Paint()
{
	int srcCol = 1;
	int srcRow = 10;
	if (m_IsSolid == false)
	{
		srcRow -= 1;
	}

	if (m_IsUsed)
	{
		srcRow = 4;
		srcCol = 4;
	}

	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y + m_yo * 3;
	SpriteSheetManager::generalTilesPtr->Paint(left, top, srcCol, srcRow);
}

void ExclamationMarkBlock::SetSolid(bool solid)
{
	m_IsSolid = solid;
	m_ActPtr->SetSensor(!solid);
}

void ExclamationMarkBlock::Hit(Level* levelPtr)
{
	SoundManager::PlaySoundEffect(SoundManager::SOUND::BLOCK_HIT);

	if (m_IsUsed == false)
	{
		m_IsUsed = true;
		m_CurrentFrameOfBumpAnimation = 2;
		m_yo = 0;

		m_ShouldSpawnSuperMushroom = true;
	}
}
