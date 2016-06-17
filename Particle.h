#pragma once

#include "Enumerations.h"
#include "AnimationInfo.h"

class Particle
{
public:
	Particle(int lifetime, DOUBLE2& positionRef);
	virtual ~Particle();

	Particle(const Particle&) = delete;
	Particle& operator=(const Particle&) = delete;

	// Returns true if this item needs to be removed
	virtual bool Tick(double deltaTime) = 0;
	virtual void Paint() = 0;

protected:
	DOUBLE2 m_Position;
	AnimationInfo m_AnimInfo;
	int m_LifeRemaining;
};
