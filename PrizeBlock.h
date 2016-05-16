#pragma once

#include "Block.h"

class PrizeBlock : public Block
{
public:
	// LATER: Find out if there are any blocks spawned as used blocks ever in the real game
	PrizeBlock(DOUBLE2 topLeft, Level* levelPtr, bool spawnsYoshi = false);

	PrizeBlock(const PrizeBlock&) = delete;
	PrizeBlock& operator=(const PrizeBlock&) = delete;

	void Tick(double deltaTime);
	void Paint();

	void Hit();
	void SetUsed(bool used);

private:
	CountdownTimer m_BumpAnimationTimer;

	int m_yo = 0;
	bool m_IsUsed = false;
	bool m_ShouldSpawnItem = false;
	bool m_SpawnsYoshi;

};