#include "stdafx.h"

#include "EnemyPoofParticle.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

StarParticle::StarParticle(DOUBLE2& positionRef, DOUBLE2& velocityRef) :
	m_Position(positionRef), m_Velocity(velocityRef)
{
}
void StarParticle::Tick(double deltaTime)
{
	m_Position += m_Velocity * deltaTime;
}
void StarParticle::Paint()
{
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::starParticlePtr, m_Position.x, m_Position.y);
}

SplatParticle::SplatParticle(DOUBLE2& positionRef) : 
	m_Position(positionRef)
{
	m_LifeRemaining = LIFETIME;
}
void SplatParticle::Tick(double deltaTime)
{
	--m_LifeRemaining;
}
void SplatParticle::Paint()
{
	MATRIX3X2 matPrevWorld = GAME_ENGINE->GetWorldMatrix();
	
	// These particles flip horizontally every two frames
	if (m_LifeRemaining % 4 >= 2)
	{
		MATRIX3X2 matTranlate = MATRIX3X2::CreateTranslationMatrix(m_Position.x, m_Position.y);
		MATRIX3X2 matReflect = MATRIX3X2::CreateScalingMatrix(-1, 1);
		GAME_ENGINE->SetWorldMatrix(matTranlate.Inverse() * matReflect * matTranlate);
	}

	GAME_ENGINE->DrawBitmap(SpriteSheetManager::splatParticlePtr, m_Position.x, m_Position.y);

	GAME_ENGINE->SetWorldMatrix(matPrevWorld);
}

EnemyDeathCloudParticle::EnemyDeathCloudParticle(DOUBLE2& positionRef) : m_Position(positionRef)
{
	m_LifeRemaining = LIFETIME;
}
void EnemyDeathCloudParticle::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	m_LifeRemaining = LIFETIME - m_AnimInfo.frameNumber - 1;
}
void EnemyDeathCloudParticle::Paint()
{
	SpriteSheetManager::enemyDeathCloudParticlePtr->Paint(m_Position.x, m_Position.y, m_AnimInfo.frameNumber, 0);
}


EnemyPoofParticle::EnemyPoofParticle(DOUBLE2& positionRef) : Particle(LIFETIME, positionRef)
{
	double xv = 55;
	double yv = 45;
	m_StarParticlePtrArr[0] = new StarParticle(positionRef, DOUBLE2(xv, yv));
	m_StarParticlePtrArr[1] = new StarParticle(positionRef, DOUBLE2(xv, -yv));
	m_StarParticlePtrArr[2] = new StarParticle(positionRef, DOUBLE2(-xv, yv));
	m_StarParticlePtrArr[3] = new StarParticle(positionRef, DOUBLE2(-xv, -yv));

	m_SplatParticlePtr = new SplatParticle(positionRef + DOUBLE2(0, -11));

	m_CloudParticlePtr = new EnemyDeathCloudParticle(positionRef);
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

	// LATER: Paint these behind the player (probably will require a seperate EnemyDeathCloud 
	// class instance which extends Particle to be added to level particles array)
	m_CloudParticlePtr->Paint();
}