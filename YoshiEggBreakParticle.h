#pragma once

#include "Particle.h"

struct ShellPiece
{
	ShellPiece() : 
		m_Acc(DOUBLE2()) 
	{
	}
	
	ShellPiece(DOUBLE2 initialPos, DOUBLE2 initialVel, DOUBLE2 acceleration) : 
		m_Pos(initialPos), m_Vel(initialVel), m_Acc(acceleration) 
	{
	}

	DOUBLE2 m_Pos;
	DOUBLE2 m_Vel;
	DOUBLE2 m_Acc;
};

class YoshiEggBreakParticle : public Particle
{
public:
	YoshiEggBreakParticle(DOUBLE2 position);
	virtual ~YoshiEggBreakParticle();

	YoshiEggBreakParticle(const YoshiEggBreakParticle&) = delete;
	YoshiEggBreakParticle& operator=(const YoshiEggBreakParticle&) = delete;

	bool Tick(double deltaTime);
	void Paint();

private:
	static const int LIFETIME = 11;
	static const int NUM_PIECES = 4;

	ShellPiece m_ShellPiecesArr[NUM_PIECES];
};
