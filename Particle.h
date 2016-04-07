#pragma once

class Particle
{
public:
	Particle(int lifetime, DOUBLE2 position);
	virtual ~Particle();

	// NOTE: Returns true if this item needs to be removed
	virtual bool Tick(double deltaTime) = 0;
	virtual void Paint() = 0;

protected:
	//Bitmap* m_BmpPtr = nullptr;
	// NOTE: Where in world space this particle is
	DOUBLE2 m_Position;

	int m_LifeRemaining;

};
