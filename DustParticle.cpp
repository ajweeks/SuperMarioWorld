#include "stdafx.h"

#include "DustParticle.h"
#include "Game.h"

DustParticle::DustParticle(DOUBLE2 position) : Particle(LIFETIME, position)
{
}

DustParticle::~DustParticle()
{
}

bool DustParticle::Tick(double deltaTime)
{
	m_LifeRemaining--;

	m_AnimInfo.Tick(deltaTime);

	return (m_LifeRemaining <= 0);
}

void DustParticle::Paint()
{
	GAME_ENGINE->FillRect(m_Position - DOUBLE2(3, 3), m_Position + DOUBLE2(6, 6));
}