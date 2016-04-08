#include "stdafx.h"

#include "CoinCollectParticle.h"
#include "Particle.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

CoinCollectParticle::CoinCollectParticle(DOUBLE2& positionRef) : Particle(LIFETIME, positionRef)
{
	m_AnimInfo.secondsPerFrame = 0.055;
}

CoinCollectParticle::~CoinCollectParticle()
{
}

bool CoinCollectParticle::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	m_LifeRemaining = LIFETIME - m_AnimInfo.frameNumber - 1;

	return (m_LifeRemaining < 0);
}

void CoinCollectParticle::Paint()
{
	SpriteSheetManager::coinCollectParticlePtr->Paint(m_Position.x, m_Position.y, m_AnimInfo.frameNumber, 0);
}