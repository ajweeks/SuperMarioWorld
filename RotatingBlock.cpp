#include "stdafx.h"

#include "RotatingBlock.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "SoundManager.h"
#include "Level.h"
#include "Beanstalk.h"

RotatingBlock::RotatingBlock(DOUBLE2 topLeft, Level* levelPtr, bool spawnsBeanstalk) :
	Block(topLeft, TYPE::ROTATING_BLOCK, levelPtr),
	m_SpawnsBeanstalk(spawnsBeanstalk)
{
	m_AnimInfo.secondsPerFrame = 0.135;

	m_RotationTimer = CountdownTimer(255);
	m_BumpAnimationTimer = CountdownTimer(14);
}

void RotatingBlock::Tick(double deltaTime)
{
	if (m_ShouldSpawnBeanstalk)
	{
		m_ShouldSpawnBeanstalk = false;

		Beanstalk* beanstalkPtr = new Beanstalk(m_ActPtr->GetPosition() - DOUBLE2(WIDTH / 2, HEIGHT), m_LevelPtr, 14);
		beanstalkPtr->AddContactListener(m_LevelPtr);
		m_LevelPtr->AddItem(beanstalkPtr);
		
		SoundManager::PlaySoundEffect(SoundManager::SOUND::BEANSTALK_SPAWN);

		return;
	}

	if (m_RotationTimer.Tick())
	{
		m_AnimInfo.Tick(deltaTime);
		m_AnimInfo.frameNumber %= 4;

		if (m_RotationTimer.IsComplete())
		{
			m_AnimInfo.frameNumber = 0;
		}
	}

	if (m_BumpAnimationTimer.Tick())
	{
		m_yo = m_BumpAnimationTimer.FramesElapsed();
		if (m_yo > m_BumpAnimationTimer.OriginalNumberOfFrames() / 2)
		{
			m_yo = (m_BumpAnimationTimer.OriginalNumberOfFrames() / 2) - (m_yo - (m_BumpAnimationTimer.OriginalNumberOfFrames() / 2));
		}
		m_yo = int(m_yo * -0.5);

		if (m_BumpAnimationTimer.IsComplete()) 
		{
			if (m_SpawnsBeanstalk)
			{
				m_ShouldSpawnBeanstalk = true;
				m_IsUsed = true;
			}
			else
			{
				m_RotationTimer.Start();
			}

			m_yo = 0;
		}
	}
}

void RotatingBlock::Paint()
{
	int srcCol = 0 + m_AnimInfo.frameNumber;
	int srcRow = 5;
	
	if (m_IsUsed)
	{
		srcCol = 4;
		srcRow = 4;
	}

	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y + m_yo * 3;

	SpriteSheetManager::generalTilesPtr->Paint(left, top, srcCol, srcRow);
}

void RotatingBlock::Hit()
{
	if (m_SpawnsBeanstalk)
	{
		if (m_IsUsed == false)
		{
			m_BumpAnimationTimer.Start();
		}
	}
	else
	{
		SoundManager::PlaySoundEffect(SoundManager::SOUND::BLOCK_HIT);
		m_BumpAnimationTimer.Start();
	}
}

bool RotatingBlock::IsRotating()
{
	return m_RotationTimer.IsActive();
}
