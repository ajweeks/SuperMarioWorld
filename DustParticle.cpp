#include "stdafx.h"

#include "DustParticle.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

DustParticle::DustParticle(DOUBLE2 position) : 
	Particle(LIFETIME, position)
{
	m_AnimInfo.secondsPerFrame = 0.08;
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
	SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::DUST_CLOUD_PARTICLE)->Paint(m_Position.x, m_Position.y, m_AnimInfo.frameNumber, 0);
}