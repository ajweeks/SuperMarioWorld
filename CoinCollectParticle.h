#pragma once

#include "Particle.h"

class CoinCollectParticle : public Particle
{
public:
	CoinCollectParticle(DOUBLE2& positionRef);
	virtual ~CoinCollectParticle();

	bool Tick(double deltaTime);
	void Paint();

private:
	static const int LIFETIME = 10;

};

