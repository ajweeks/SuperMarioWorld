#include "stdafx.h"

#include "RotatingBlock.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "SoundManager.h"

RotatingBlock::RotatingBlock(DOUBLE2 topLeft, Level* levelPtr) :
	Block(topLeft, TYPE::ROTATING_BLOCK, levelPtr)
{
}

void RotatingBlock::Tick(double deltaTime)
{
	if (m_IsRotating)
	{
		m_AnimInfo.Tick(deltaTime);
		m_AnimInfo.frameNumber %= 4;
		if (m_AnimInfo.frameNumber == 3)
		{
			m_Rotations++;
			if (m_Rotations > MAX_ROTATIONS)
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
	double srcCol = 0 + m_AnimInfo.frameNumber;
	double srcRow = 5;
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	SpriteSheetManager::generalTilesPtr->Paint(left, top, srcCol, srcRow);
}

void RotatingBlock::Hit(Level* levelPtr)
{
	SoundManager::PlaySoundEffect(SoundManager::SOUND::BLOCK_HIT);
	m_IsRotating = true;
}

bool RotatingBlock::IsRotating()
{
	return m_IsRotating;
}
