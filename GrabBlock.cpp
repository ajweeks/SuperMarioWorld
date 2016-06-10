#include "stdafx.h"

#include "GrabBlock.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "SoundManager.h"
#include "Level.h"
#include "EnemyDeathCloudParticle.h"

GrabBlock::GrabBlock(DOUBLE2 topLeft, Level* levelPtr) :
	Block(topLeft, Type::GRAB_BLOCK, levelPtr, BodyType::KINEMATIC)
{
	m_AnimInfo.secondsPerFrame = 0.03;
	m_LifeRemaining = SMWTimer(500);
}

void GrabBlock::Tick(double deltaTime)
{
	if (m_LifeRemaining.Tick() && m_LifeRemaining.IsComplete())
	{
		EnemyDeathCloudParticle* cloudParticlePtr = new EnemyDeathCloudParticle(m_ActPtr->GetPosition());
		m_LevelPtr->AddParticle(cloudParticlePtr);

		m_LevelPtr->RemoveItem(this);
		return;
	}

	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 4;
}

void GrabBlock::Paint()
{
	int srcCol = 0;
	int srcRow = 7;

	if (m_IsFlashing)
	{
		srcCol = 0 + m_AnimInfo.frameNumber;
	}

	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES)->Paint(left, top, srcCol, srcRow);
}

void GrabBlock::Hit()
{
	// These blocks don't respond to being hit, only grabbed
}

void GrabBlock::Grab()
{
	if (m_IsFlashing == false)
	{
		m_IsFlashing = true;
		m_LifeRemainingTimer.Start();
	}
}

void GrabBlock::SetMoving(DOUBLE2 velocity)
{
	m_IsMoving  = (velocity == DOUBLE2(0.0, 0.0));
	m_ActPtr->SetLinearVelocity(velocity);
}
