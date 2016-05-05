#include "stdafx.h"

#include "Coin.h"
#include "Level.h"
#include "NumberParticle.h"
#include "CoinCollectParticle.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

// NOTE: If 'life == -1', this coin has an infinite lifetime (until the player picks it up), 
// otherwise it lives for 'life' ticks before it is removed
Coin::Coin(DOUBLE2 topLeft, Level* levelPtr, int life, TYPE type, DOUBLE2 size) :
	Item(topLeft, type, levelPtr, BodyType::STATIC, int(size.x), int(size.y)),
	m_Life(life)
{
	m_ActPtr->SetSensor(true);

	m_AnimInfo.secondsPerFrame = 0.15;

	if (life > -1)
	{
		// This coin shoots up, then falls down, then disapears
		m_ActPtr->SetBodyType(BodyType::DYNAMIC);
		m_ActPtr->SetLinearVelocity(DOUBLE2(0, -250));
	}
}

void Coin::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 4;

	if (m_Life > -1)
	{
		if (m_Life == LIFETIME)
		{
			// NOTE: Give the player the coin right away so that the sound effect play now
			m_LevelPtr->GiveItemToPlayer(this);
		}
		if (--m_Life <= 0)
		{
			GenerateParticles();
			m_LevelPtr->RemoveItem(this);
		}
	}
}

int Coin::GetLifeRemaining()
{
	return m_Life;
}

void Coin::GenerateParticles()
{
	NumberParticle* numberParticlePtr = new NumberParticle(10, m_ActPtr->GetPosition() + DOUBLE2(5, -12));
	CoinCollectParticle* coinParticlePtr = new CoinCollectParticle(m_ActPtr->GetPosition() + DOUBLE2(0, -5));

	m_LevelPtr->AddParticle(numberParticlePtr);
	m_LevelPtr->AddParticle(coinParticlePtr);
}

void Coin::Paint()
{
	int srcCol = 0 + m_AnimInfo.frameNumber;
	int srcRow = 0;
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	SpriteSheetManager::generalTilesPtr->Paint(left, top, srcCol, srcRow);
}
