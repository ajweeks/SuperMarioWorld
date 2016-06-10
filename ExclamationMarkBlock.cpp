#include "stdafx.h"

#include "ExclamationMarkBlock.h"
#include "SuperMushroom.h"
#include "Level.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "SoundManager.h"

ExclamationMarkBlock::ExclamationMarkBlock(DOUBLE2 topLeft, Colour colour, bool isSolid, Level* levelPtr):
	Block(topLeft, Type::EXCLAMATION_MARK_BLOCK, levelPtr), m_Colour(colour)
{
	SetSolid(isSolid);
	m_BumpAnimationTimer = SMWTimer(14);
}

void ExclamationMarkBlock::Tick(double deltaTime)
{
	if (m_ShouldSpawnSuperMushroom)
	{
		Item* newItem = new SuperMushroom(m_ActPtr->GetPosition() - DOUBLE2(SuperMushroom::WIDTH / 2, HEIGHT / 2 + 2), m_LevelPtr);
		m_LevelPtr->AddItem(newItem, true);

		m_ShouldSpawnSuperMushroom = false;
	}

	if (m_BumpAnimationTimer.Tick())
	{
		m_yo = m_BumpAnimationTimer.FramesElapsed();
		if (m_yo > m_BumpAnimationTimer.TotalFrames() / 2)
		{
			m_yo = (m_BumpAnimationTimer.TotalFrames() / 2) - (m_yo - (m_BumpAnimationTimer.TotalFrames() / 2));
		}
		m_yo = int(m_yo * -0.5);

		if (m_BumpAnimationTimer.IsComplete())
		{
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

	if (m_BumpAnimationTimer.IsActive())
	{
		srcRow = 5;
		srcCol = 0;
	}
	else if (m_IsUsed)
	{
		srcRow = 4;
		srcCol = 5;
	}

	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y + m_yo * 3;
	SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES)->Paint(left, top, srcCol, srcRow);
}

void ExclamationMarkBlock::SetSolid(bool solid)
{
	m_IsSolid = solid;
	m_ActPtr->SetSensor(!solid);
}

void ExclamationMarkBlock::Hit()
{
	if (m_IsUsed == false)
	{
		SoundManager::PlaySoundEffect(SoundManager::Sound::SUPER_MUSHROOM_SPAWN);

		m_IsUsed = true;
		m_BumpAnimationTimer.Start();
		m_yo = 0;

		m_ShouldSpawnSuperMushroom = true;
	}
	else
	{
		SoundManager::PlaySoundEffect(SoundManager::Sound::BLOCK_HIT);
	}
}
