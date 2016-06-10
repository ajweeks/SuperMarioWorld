#pragma once

#include "Block.h"

class ExclamationMarkBlock : public Block
{
public:
	ExclamationMarkBlock(DOUBLE2 topLeft, Colour colour, bool isSolid, Level* levelPtr);

	ExclamationMarkBlock(const ExclamationMarkBlock&) = delete;
	ExclamationMarkBlock& operator=(const ExclamationMarkBlock&) = delete;
	
	void Tick(double deltaTime);
	void Paint();
	void SetSolid(bool solid);
	void Hit();

private:
	Colour m_Colour;
	SMWTimer m_BumpAnimationTimer;
	int m_yo = 0; // NOTE: Used for the bump animation
				  // NOTE: This is set to true for every ! block when the player hits the
				  // yellow switch palace switch block
	bool m_IsSolid = false;
	bool m_IsUsed = false;
	bool m_ShouldSpawnSuperMushroom = false;

};