#pragma once

#include "Block.h"

class PrizeBlock : public Block
{
public:
	// LATER: Find out if there are any blocks spawned as used blocks ever in the real game
	PrizeBlock(DOUBLE2 topLeft, Level* levelPtr, std::string spawnTypeStr = "", bool isFlyer = false);

	PrizeBlock(const PrizeBlock&) = delete;
	PrizeBlock& operator=(const PrizeBlock&) = delete;

	void Tick(double deltaTime);
	void Paint();

	void Hit();
	void SetUsed(bool used);

	bool IsFlying() const;

private:
	SMWTimer m_BumpAnimationTimer;
	int m_yo = 0;
	bool m_IsUsed = false;
	bool m_ShouldSpawnItem = false;
	const std::string m_SpawnTypeStr; // What type of thing we spawn (yoshi or 1-UP), if empty then we spawn a coin

	const bool m_IsFlyer = false; // Some underground prize blocks have wings
	bool m_IsFlying = false;	  // Once the player hits a flying block, they no longer fly around
	bool m_ShouldFreeze = false;  // True once we get hit, to signal that we should change our body type to static
	double m_LifeElapsed = 0.0;   // Flying blocks need to know how old they are
};
