#pragma once

#include "Particle.h"

class OneUpParticle : public Particle
{
public:
	OneUpParticle(DOUBLE2 position);
	virtual ~OneUpParticle();

	OneUpParticle(const OneUpParticle&) = delete;
	OneUpParticle& operator=(const OneUpParticle&) = delete;

	bool Tick(double deltaTime);
	void Paint();

private:
	static const int LIFETIME = 44;

	static const int FAST_VEL = 40;
	static const int SLOW_VEL = 20;
};

