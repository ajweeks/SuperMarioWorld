#include "stdafx.h"

#include "Particle.h"
#include "Game.h"

Particle::Particle(int lifetime, DOUBLE2& positionRef) : 
	m_LifeRemaining(lifetime), m_Position(positionRef)
{
}

Particle::~Particle()
{
}
