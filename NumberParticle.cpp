#include "stdafx.h"

#include "NumberParticle.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

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

	if (m_LifeRemaining < LIFETIME / 2)
	{
		m_Position.y -= 20 * deltaTime;
	}
	else
	{
		m_Position.y -= 40 * deltaTime;
	}

	return (m_LifeRemaining <= 0);
}

void PaintSeveralDigitNumber(int x, int y, int number)
{
	number = abs(number);

	do {
		int digit = number % 10;
		SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::NUMBER_PARTICLE)->Paint(x, y, digit, 0);

		x -= 4;
		number /= 10;
	} while (number > 0);
}

void NumberParticle::Paint()
{
	PaintSeveralDigitNumber(int(m_Position.x), int(m_Position.y), m_Value);
}