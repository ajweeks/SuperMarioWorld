#pragma once

#include "Particle.h"

class NumberParticle : public Particle
{
	friend void PaintSeveralDigitNumber();

public:
	NumberParticle(int value, DOUBLE2 position);
	virtual ~NumberParticle();

	bool Tick(double deltaTime);
	void Paint();

private:
	static const int LIFETIME = 25;

	int m_Value;
	DOUBLE2 m_Velocity;
};

