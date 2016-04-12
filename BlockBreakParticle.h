#pragma once

#include "Particle.h"

class BlockChunk
{
public:
	BlockChunk(DOUBLE2& positionRef, DOUBLE2& velocityRef);

	void Tick(double deltaTime);
	void Paint();

private:
	RECT2 GetBlockParticleSrcRect(int index);

	DOUBLE2 m_Position;
	DOUBLE2 m_Velocity;
	double m_BlockType;
	double m_TypeTimer;

};

class BlockBreakParticle : public Particle
{
public:
	BlockBreakParticle(DOUBLE2& positionRef);
	virtual ~BlockBreakParticle();

	bool Tick(double deltaTime);
	void Paint();

private:
	static const int LIFETIME = 100;
	

	// Holds 4 values, one for each block piece
	BlockChunk* m_BlockChunkPtrArr[4];

};

