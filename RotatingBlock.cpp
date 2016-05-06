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

	if (m_IsRotating)
	{
		m_AnimInfo.Tick(deltaTime);
		m_AnimInfo.frameNumber %= 4;
		if (m_AnimInfo.frameNumber == 3)
		{
			m_Rotations++;
			if (m_Rotations > FRAMES_OF_ROTATION)
			{
				m_IsRotating = false;
				m_Rotations = 0;
				m_AnimInfo.frameNumber = 0;
			}
		}
	}
}

void RotatingBlock::Paint()
{
	int srcCol = 0 + m_AnimInfo.frameNumber;
	int srcRow = 5;
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	SpriteSheetManager::generalTilesPtr->Paint(left, top, srcCol, srcRow);
}

void RotatingBlock::Hit(Level* levelPtr)
{
	if (m_SpawnsBeanstalk)
	{
		 m_ShouldSpawnBeanstalk = true;
	}
	else
	{
		SoundManager::PlaySoundEffect(SoundManager::SOUND::BLOCK_HIT);
		m_IsRotating = true;
	}
}

bool RotatingBlock::IsRotating()
{
	return m_IsRotating;
}
