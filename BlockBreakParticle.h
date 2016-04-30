#pragma once

#include "Particle.h"

class BlockChunk
{
public:
	BlockChunk(DOUBLE2 position, DOUBLE2 velocity);

	BlockChunk(const BlockChunk&) = delete;
	BlockChunk& operator=(const BlockChunk&) = delete;

	void Tick(double deltaTime);
	void Paint();

private:
	RECT2 GetBlockParticleSrcRect(int index);

	DOUBLE2 m_Position;
	DOUBLE2 m_Velocity;
	int m_BlockType;
	int m_TypeTimer;

};



class BlockBreakParticle : public Particle
{
public:
	BlockBreakParticle(DOUBLE2 centerPos);
	virtual ~BlockBreakParticle();

	BlockBreakParticle(const BlockBreakParticle&) = delete;
	BlockBreakParticle&operator=(const BlockBreakParticle&) = delete;

	bool Tick(double deltaTime);
	void Paint();

private:
	static const int LIFETIME = 100;
	

	// Holds 4 values, one for each block piece
	BlockChunk* m_BlockChunkPtrArr[4];

};

