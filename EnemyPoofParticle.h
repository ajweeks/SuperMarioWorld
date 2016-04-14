#pragma once

#include "Particle.h"

class StarParticle
{
public:
	StarParticle(DOUBLE2& positionRef, DOUBLE2& velocityRef);

	void Tick(double deltaTime);
	void Paint();

private:
	DOUBLE2 m_Position;
	DOUBLE2 m_Velocity;
};

class SplatParticle
{
public:
	SplatParticle(DOUBLE2& positionRef);

	void Tick(double deltaTime);
	void Paint();

private:
	static const int LIFETIME = 7;

	int m_LifeRemaining;
	DOUBLE2 m_Position;
};

class EnemyDeathCloudParticle
{
public:
	EnemyDeathCloudParticle(DOUBLE2& positionRef);

	void Tick(double deltaTime);
	void Paint();

private:
	static const int LIFETIME = 8;

	ANIMATION_INFO m_AnimInfo;
	DOUBLE2 m_Position;
	int m_LifeRemaining;
};

// When the player spin jumps on a enemy's head 3 particles are spawned,
// a cloud, 4 outward shooting stars, and a white splat
// There is also a points particle spawned, but this class is not responsible for that
class EnemyPoofParticle : public Particle
{
public:
	EnemyPoofParticle(DOUBLE2& positionRef);
	virtual ~EnemyPoofParticle();

	bool Tick(double deltaTime);
	void Paint();

private:
	static const int LIFETIME = 12;

	StarParticle* m_StarParticlePtrArr[4];
	SplatParticle* m_SplatParticlePtr = nullptr;
	EnemyDeathCloudParticle* m_CloudParticlePtr = nullptr;

};

