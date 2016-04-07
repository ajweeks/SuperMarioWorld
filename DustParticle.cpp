#include "stdafx.h"

#include "DustParticle.h"
#include "Game.h"
#include "SpriteSheetManager.h"

DustParticle::DustParticle(DOUBLE2& positionRef) : Particle(LIFETIME, positionRef)
{
	m_AnimInfo.msPerFrame = 0.11;
}

DustParticle::~DustParticle()
{
}

bool DustParticle::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	m_LifeRemaining = LIFETIME - m_AnimInfo.frameNumber - 1;

	return (m_LifeRemaining < 0);
}

void DustParticle::Paint()
{
	SpriteSheetManager::runningDustCloudParticle->Paint(m_Position.x, m_Position.y, m_AnimInfo.frameNumber, 0);
}