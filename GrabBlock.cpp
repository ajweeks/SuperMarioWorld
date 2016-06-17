#include "stdafx.h"

#include "GrabBlock.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "SoundManager.h"
#include "Level.h"
#include "EnemyDeathCloudParticle.h"
#include "SplatParticle.h"
#include "Player.h"
#include "BlockBreakParticle.h"

const double GrabBlock::VERTICAL_KICK_VEL = -29000;

GrabBlock::GrabBlock(DOUBLE2 topLeft, Level* levelPtr) :
	Block(topLeft, Type::GRAB_BLOCK, levelPtr, BodyType::STATIC, WIDTH, HEIGHT)
{
	b2Filter collisionFilter = m_ActPtr->GetCollisionFilter();
	collisionFilter.maskBits |= Level::PLAYER | Level::SHELL;
	m_ActPtr->SetCollisionFilter(collisionFilter);

	m_AnimInfo.secondsPerFrame = 0.02;
	m_LifeRemaining = SMWTimer(500);
}

GrabBlock::~GrabBlock()
{
}

void GrabBlock::Tick(double deltaTime)
{
	Player* playerPtr = m_LevelPtr->GetPlayer();
	if (m_ShouldBeRemoved)
	{
		if (playerPtr->GetHeldItemPtr() == this)
		{
			playerPtr->DropHeldItem();
		}
		m_LevelPtr->RemoveItem(this);
		return;
	}

	if (m_LifeRemaining.Tick() && m_LifeRemaining.IsComplete())
	{
		EnemyDeathCloudParticle* cloudParticlePtr = new EnemyDeathCloudParticle(m_ActPtr->GetPosition());
		m_LevelPtr->AddParticle(cloudParticlePtr);

		if (playerPtr->GetHeldItemPtr() == this)
		{
			playerPtr->DropHeldItem();
		}
		m_LevelPtr->RemoveItem(this);
		return;
	}

	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 6;
}

void GrabBlock::Paint()
{
	int srcCol = 0;
	int srcRow = 11;

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
	m_IsFlashing = true;
	if (m_LifeRemaining.IsActive() == false)
	{
		m_LifeRemaining.Start();
	}
	m_ActPtr->SetBodyType(BodyType::DYNAMIC);
}

void GrabBlock::SetMoving(DOUBLE2 velocity)
{
	m_IsMoving = true;
	m_ActPtr->GetBody()->GetFixtureList()->SetFriction(0.0);
	m_ActPtr->SetLinearVelocity(velocity);
}

void GrabBlock::KickVertically(double deltaTime, double horizontalVel)
{
	SoundManager::PlaySoundEffect(SoundManager::Sound::SHELL_KICK);

	SplatParticle* splatParticlePtr = new SplatParticle(m_ActPtr->GetPosition());
	m_LevelPtr->AddParticle(splatParticlePtr);

	m_ActPtr->SetLinearVelocity(DOUBLE2(horizontalVel, VERTICAL_KICK_VEL * deltaTime));
}

bool GrabBlock::IsFlashing() const
{
	return m_IsFlashing;
}

void GrabBlock::Explode()
{
	BlockBreakParticle* blockBreakParticle = new BlockBreakParticle(m_ActPtr->GetPosition(), true);
	m_LevelPtr->AddParticle(blockBreakParticle);

	SoundManager::PlaySoundEffect(SoundManager::Sound::BLOCK_BREAK);

	m_ShouldBeRemoved = true;
}
