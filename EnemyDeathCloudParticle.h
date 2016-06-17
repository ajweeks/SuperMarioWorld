#pragma once

#include "Particle.h"

class EnemyDeathCloudParticle : public Particle
{
public:
	EnemyDeathCloudParticle(DOUBLE2 position);
	virtual ~EnemyDeathCloudParticle();

	EnemyDeathCloudParticle(const EnemyDeathCloudParticle&) = delete;
	EnemyDeathCloudParticle& operator=(const EnemyDeathCloudParticle&) = delete;

	bool Tick(double deltaTime);
	void Paint();

private:
	static const int LIFETIME = 5;
};
