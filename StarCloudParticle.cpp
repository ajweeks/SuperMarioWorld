#include "stdafx.h"

#include "StarCloudParticle.h"
#include "Particle.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "Game.h"

StarCloudParticle::StarCloudParticle(DOUBLE2 position) : Particle(LIFETIME, position)
{
}

StarCloudParticle::~StarCloudParticle()
{
}

bool StarCloudParticle::Tick(double deltaTime)
{
	return (--m_LifeRemaining < 0);
}

void StarCloudParticle::Paint()
{
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::STAR_CLOUD_PARTICLE), m_Position.x, m_Position.y);
}
