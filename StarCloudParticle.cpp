#include "stdafx.h"

#include "StarCloudParticle.h"
#include "Particle.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "Game.h"

StarCloudParticle::StarCloudParticle(DOUBLE2 position) : Particle(LIFETIME, position)
{
	m_AnimInfo.secondsPerFrame = 0.055;
}

StarCloudParticle::~StarCloudParticle()
{
}

bool StarCloudParticle::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	m_LifeRemaining = LIFETIME - m_AnimInfo.frameNumber - 1;

	return (m_LifeRemaining < 0);
}

void StarCloudParticle::Paint()
{
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::STAR_CLOUD_PARTICLE), m_Position.x, m_Position.y);
}