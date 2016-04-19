#pragma once

#include "Particle.h"

class EnemyDeathCloudParticle : public Particle
{
public:
	EnemyDeathCloudParticle(DOUBLE2& positionRef);

	bool Tick(double deltaTime);
	void Paint();

private:
	static const int LIFETIME = 8;
};
