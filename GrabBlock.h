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
	void KickVertically(double deltaTime, double horizontalVel);
	bool IsFlashing() const;
	void Explode();

	static const int WIDTH = 14;
	static const int HEIGHT = 14;

private:
	static const double VERTICAL_KICK_VEL;

	bool m_IsFlashing = false; // True once the player picks us up
	bool m_IsMoving = false; // True after the player kicks us
	SMWTimer m_LifeRemaining; // This starts counting down when the player picks us up
	bool m_ShouldBeRemoved = false;
};
