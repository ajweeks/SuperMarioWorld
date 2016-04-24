#pragma once

#include "Item.h"
#include "Enumerations.h"

class KoopaShell : public Item
{
public:	
	KoopaShell(DOUBLE2 topLeft, Level* levelPtr, COLOUR colour);
	virtual ~KoopaShell();

	void Tick(double deltaTime);
	void Paint();

	// NOTE: wasThrown is true when this shell was being held by Mario and was then thrown
	// An extra splat particle is generated
	void KickHorizontally(int facingDir, bool wasThrown);
	void KickVertically(double horizontalVel);
	void ShellHit();
	void Stomp();

	bool IsFalling();

	bool IsMoving();
	void SetMoving(bool moving);
	bool IsBouncing();
private:
	// NOTE: This value is affected by how fast the player is moving
	// This is the minimum value (when the player is standing still)
	static const double HORIZONTAL_KICK_BASE_VEL;
	static const double VERTICAL_KICK_VEL;
	static const double SHELL_HIT_VEL;

	COLOUR m_Colour;
	// NOTE: Moving is true when this shell is sliding on the ground, and animating
	bool m_IsMoving = false;
	int m_DirMoving;

	// NOTE: This is true when this shell has been thrown into the air by the player
	// and hasn't hit the ground yet
	bool m_IsBouncing = false;

	bool m_IsFalling = false;

	bool m_ShouldBeRemoved = false;
};
