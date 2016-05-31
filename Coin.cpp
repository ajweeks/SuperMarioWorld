#include "stdafx.h"

#include "Coin.h"
#include "Level.h"
#include "CoinCollectParticle.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

// NOTE: If 'life == -1', this coin has an infinite lifetime (until the player picks it up), 
// otherwise it lives for 'life' ticks before it is removed
Coin::Coin(DOUBLE2 topLeft, Level* levelPtr, int life, Type type, DOUBLE2 size) :
	Item(topLeft, type, levelPtr, Level::ITEM, BodyType::STATIC, int(size.x), int(size.y))
{
	m_ActPtr->SetSensor(true);

	m_AnimInfo.secondsPerFrame = 0.15;

	if (life > -1)
	{
		m_LifeRemaining = CountdownTimer(LIFETIME);
		m_LifeRemaining.Start();

		// This coin shoots up, then falls down, then disapears
		m_ActPtr->SetBodyType(BodyType::DYNAMIC);
		m_ActPtr->SetLinearVelocity(DOUBLE2(0, INITIAL_Y_VEL));
	}

	m_IsActive = true;
}

void Coin::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 4;

	if (m_LifeRemaining.Tick())
	{
		if (m_LifeRemaining.FramesElapsed() == 1)
		{
			// NOTE: Give the player the coin right away so that the sound effect play now
			m_LevelPtr->GiveItemToPlayer(this);
		}
		if (m_LifeRemaining.IsComplete())
		{
			GenerateParticles();
			m_LevelPtr->RemoveItem(this);
		}
	}
}

bool Coin::HasInfiniteLifetime()
{
	return m_LifeRemaining.IsActive() == false;
}

void Coin::GenerateParticles()
{
	CoinCollectParticle* coinParticlePtr = new CoinCollectParticle(m_ActPtr->GetPosition() + DOUBLE2(0, -5));
	m_LevelPtr->AddParticle(coinParticlePtr);
}

void Coin::Paint()
{
	int srcCol = 0 + m_AnimInfo.frameNumber;
	int srcRow = 0;
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;

	if (m_IsBlock)
	{
		srcRow = 4;
		srcCol = 4;
	}

	SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES)->Paint(left, top, srcCol, srcRow);
}

// If toBlock is false, then we are being turned back to a coin
void Coin::TurnToBlock(bool toBlock)
{
	m_ActPtr->SetSensor(!toBlock);
	m_IsBlock = toBlock;
}

bool Coin::IsBlock()
{
	return m_IsBlock;
}
