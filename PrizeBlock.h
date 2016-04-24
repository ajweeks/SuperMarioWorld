#pragma once

#include "Block.h"

class PrizeBlock : public Block
{
public:
	// LATER: Find out if there are any blocks spawned as used blocks ever in the real game
	PrizeBlock(DOUBLE2 topLeft, Level* levelPtr, bool spawnsYoshi = false);
	void Tick(double deltaTime);
	void Paint();
	// NOTE: I think Box2D prevents you from adding physics actors during preSolve, therefore
	// we need to set a flag when the player hits a prize block and generate the coin in the next tick,
	// or at least after pre-solve
	void Hit(Level* levelPtr);
	void SetUsed(bool used);

private:
	int m_CurrentFrameOfBumpAnimation = -1;
	int m_FramesOfBumpAnimation = 14;
	int m_yo = 0;
	bool m_IsUsed = false;
	bool m_ShouldSpawnItem = false;
	bool m_SpawnsYoshi;

};