#include "stdafx.h"

#include "OneUpParticle.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

OneUpParticle::OneUpParticle(DOUBLE2 position) :
	Particle(LIFETIME, position)
{
}

OneUpParticle::~OneUpParticle()
{
}

bool OneUpParticle::Tick(double deltaTime)
{
	m_LifeRemaining--;

	if (m_LifeRemaining < LIFETIME / 2)
	{
		m_Position.y -= SLOW_VEL * deltaTime;
	}
	else
	{
		m_Position.y -= FAST_VEL * deltaTime;
	}

	return (m_LifeRemaining <= 0);
}

void OneUpParticle::Paint()
{
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::oneUpParticlePtr, m_Position);
}