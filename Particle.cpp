#include "stdafx.h"

#include "Particle.h"
#include "Game.h"

Particle::Particle(int lifetime, DOUBLE2 position) : m_LifeRemaining(lifetime), m_Position(position)
{
}

Particle::~Particle()
{
}
