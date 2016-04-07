#include "stdafx.h"

#include "NumberParticle.h"
#include "Game.h"
#include "SpriteSheetManager.h"

NumberParticle::NumberParticle(int value, DOUBLE2 position) : 
	Particle(LIFETIME, position), m_Value(value)
{
}

NumberParticle::~NumberParticle()
{
}

bool NumberParticle::Tick(double deltaTime)
{
	m_LifeRemaining--;

	m_Position.y -= 45 * deltaTime;

	return (m_LifeRemaining <= 0);
}

void NumberParticle::Paint()
{
	PaintSeveralDigitNumber(int(m_Position.x), int(m_Position.y), m_Value, false);
}