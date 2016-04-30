#pragma once

#include "Particle.h"

class StarParticle : public Particle
{
public:
	StarParticle(DOUBLE2& positionRef, DOUBLE2& velocityRef);

	StarParticle(const StarParticle&) = delete;
	StarParticle& operator=(const StarParticle&) = delete;

	bool Tick(double deltaTime);
	void Paint();

private:
	static const int LIFETIME = 8;

	DOUBLE2 m_Velocity;
};
