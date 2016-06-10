#pragma once

#include "Block.h"

class GrabBlock : public Block
{
public:
	GrabBlock(DOUBLE2 topLeft, Level* levelPtr);

	GrabBlock(const GrabBlock&) = delete;
	GrabBlock& operator=(const GrabBlock&) = delete;

	void Tick(double deltaTime);
	void Paint();

	void Hit();
	void Grab();

	void SetMoving(DOUBLE2 velocity);

private:

	bool m_IsFlashing = false; // True once the player picks us up
	bool m_IsMoving = false; // True once the player picks us up and then releases us
	SMWTimer m_LifeRemaining; // This starts counting down when the player picks us up
};
