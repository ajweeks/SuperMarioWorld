#pragma once

#include "Particle.h"

class SplatParticle : public Particle
{
public:
	SplatParticle(DOUBLE2 position);

	bool Tick(double deltaTime);
	void Paint();

private:
	static const int LIFETIME = 12;

	static const int WIDTH = 17;
	static const int HEIGHT = 17;
};
