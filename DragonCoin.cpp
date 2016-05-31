#include "stdafx.h"

#include "DragonCoin.h"
#include "CoinCollectParticle.h"
#include "Level.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

// NOTE: Dragon coins always haev infinite life, therefore -1 as life param
DragonCoin::DragonCoin(DOUBLE2 centerPos, Level* levelPtr) :
	Coin(centerPos, levelPtr, -1, Type::DRAGON_COIN, DOUBLE2(WIDTH, HEIGHT))
{
}

void DragonCoin::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 6;
}

void DragonCoin::GenerateParticles()
{
	CoinCollectParticle* coinParticlePtr = new CoinCollectParticle(m_ActPtr->GetPosition() + DOUBLE2(0, -5));
	m_LevelPtr->AddParticle(coinParticlePtr);
}

void DragonCoin::Paint()
{
	int srcCol = 0 + m_AnimInfo.frameNumber;
	int srcRow = 2;
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	// NOTE: Dragon coins are two tiles tall, we could draw both at once of course 
	// but drawing each half seperately is perhaps a bit simpler
	SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES)->Paint(left, top, srcCol, srcRow);
	SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES)->Paint(left, top + Coin::HEIGHT, srcCol, srcRow + 1);
}
