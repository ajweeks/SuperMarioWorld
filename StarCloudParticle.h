#pragma once

#include "Particle.h"

class StarCloudParticle : public Particle
{
public:
	StarCloudParticle(DOUBLE2 position);
	virtual ~StarCloudParticle();

	StarCloudParticle(const StarCloudParticle&) = delete;
	StarCloudParticle& operator=(const StarCloudParticle&) = delete;

	bool Tick(double deltaTime);
	void Paint();

private:
	static const int LIFETIME = 8;
};
