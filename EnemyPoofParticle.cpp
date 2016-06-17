#include "stdafx.h"

#include "EnemyPoofParticle.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "StarParticle.h"
#include "SplatParticle.h"
#include "EnemyDeathCloudParticle.h"

EnemyPoofParticle::EnemyPoofParticle(DOUBLE2 position) : 
	Particle(LIFETIME, position)
{
	double xv = 55;
	double yv = 45;
	m_StarParticlePtrArr[0] = new StarParticle(position, DOUBLE2(xv, yv));
	m_StarParticlePtrArr[1] = new StarParticle(position, DOUBLE2(xv, -yv));
	m_StarParticlePtrArr[2] = new StarParticle(position, DOUBLE2(-xv, yv));
	m_StarParticlePtrArr[3] = new StarParticle(position, DOUBLE2(-xv, -yv));

	m_SplatParticlePtr = new SplatParticle(position + DOUBLE2(0, -11));

	m_CloudParticlePtr = new EnemyDeathCloudParticle(position);
}

EnemyPoofParticle::~EnemyPoofParticle()
{
	for (int i = 0; i < 4; ++i)
	{
		delete m_StarParticlePtrArr[i];
	}

	delete m_SplatParticlePtr;
	delete m_CloudParticlePtr;
}

bool EnemyPoofParticle::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	--m_LifeRemaining;
	
	for (int i = 0; i < 4; ++i)
	{
		m_StarParticlePtrArr[i]->Tick(deltaTime);
	}

	m_SplatParticlePtr->Tick(deltaTime);
	m_CloudParticlePtr->Tick(deltaTime);

	return (m_LifeRemaining < 0);
}

void EnemyPoofParticle::Paint()
{
	for (int i = 0; i < 4; ++i)
	{
		m_StarParticlePtrArr[i]->Paint();
	}
	
	m_SplatParticlePtr->Paint();
	m_CloudParticlePtr->Paint();
}
