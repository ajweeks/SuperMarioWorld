#pragma once

#include "Particle.h"

class NumberParticle : public Particle
{
public:
	NumberParticle(int value, DOUBLE2 position);
	virtual ~NumberParticle();

	NumberParticle(const NumberParticle&) = delete;
	NumberParticle& operator=(const NumberParticle&) = delete;

	bool Tick(double deltaTime);
	void Paint();

private:
	static const int LIFETIME = 44;

	int m_Value;
	DOUBLE2 m_Velocity;
};

