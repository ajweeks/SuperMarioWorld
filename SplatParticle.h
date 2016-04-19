#pragma once

#include "Particle.h"

class SplatParticle : public Particle
{
public:
	SplatParticle(DOUBLE2& positionRef);

	bool Tick(double deltaTime);
	void Paint();

private:
	static const int LIFETIME = 7;
};
