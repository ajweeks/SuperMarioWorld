#include "stdafx.h"

#include "EnemyDeathCloudParticle.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

EnemyDeathCloudParticle::EnemyDeathCloudParticle(DOUBLE2 position) : 
	Particle(LIFETIME, position)
{
	m_LifeRemaining = LIFETIME;
	m_AnimInfo.secondsPerFrame = 0.1;
}

bool EnemyDeathCloudParticle::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	m_LifeRemaining = LIFETIME - m_AnimInfo.frameNumber - 1;

	return m_LifeRemaining < 0;
}

void EnemyDeathCloudParticle::Paint()
{
	SpriteSheetManager::enemyDeathCloudParticlePtr->Paint(m_Position.x, m_Position.y, m_AnimInfo.frameNumber, 0);
}
