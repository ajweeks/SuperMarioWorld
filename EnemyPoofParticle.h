#pragma once

#include "Particle.h"

class EnemyDeathCloudParticle;
class SplatParticle;
class StarParticle;

// When the player spin jumps on a enemy's head 3 particles are spawned,
// a cloud, 4 outward shooting stars, and a white splat
// There is also a points particle spawned, but this class is not responsible for that
class EnemyPoofParticle : public Particle
{
public:
	EnemyPoofParticle(DOUBLE2 position);
	virtual ~EnemyPoofParticle();

	EnemyPoofParticle(const EnemyPoofParticle&) = delete;
	EnemyPoofParticle& operator=(const EnemyPoofParticle&) = delete;

	bool Tick(double deltaTime);
	void Paint();

private:
	static const int LIFETIME = 12;

	StarParticle* m_StarParticlePtrArr[4];
	SplatParticle* m_SplatParticlePtr = nullptr;
	EnemyDeathCloudParticle* m_CloudParticlePtr = nullptr;

};
