#pragma once

#include "Particle.h"
#include "Enumerations.h"

class DustParticle : public Particle
{
public:
	DustParticle(DOUBLE2 position);
	virtual ~DustParticle();

	bool Tick(double deltaTime);
	void Paint();

private:
	static const int LIFETIME = 4; // NOTE: Once we show our four frames, we're gone

	ANIMATION_INFO m_AnimInfo;

};

