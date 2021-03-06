#pragma once

#include "Item.h"
#include "Enumerations.h"

class KoopaShell : public Item
{
public:	
	KoopaShell(DOUBLE2 topLeft, Level* levelPtr, Colour colour, bool upsideDown = false);
	virtual ~KoopaShell();

	KoopaShell(const KoopaShell&) = delete;
	KoopaShell& operator=(const KoopaShell&) = delete;

	void Tick(double deltaTime);
	void Paint();

	void KickHorizontally(int facingDir, bool wasThrown);
	void KickVertically(double deltaTime, double horizontalVel);
	void ShellHit(int dirX = 0);
	void Stomp();

	bool IsFallingOffScreen();

	bool IsMoving();
	void SetMoving(bool moving, int direction = 0);
	bool IsBouncing();

	Colour GetColour();

	static const int WIDTH = 14;
	static const int HEIGHT = 14;

private:
	// NOTE: This value is affected by how fast the player is moving
	// This is the minimum value (when the player is standing still)
	static const double HORIZONTAL_KICK_BASE_VEL;
	static const double VERTICAL_KICK_VEL;
	static const double SHELL_HIT_VEL;
	static const double HORIZONTAL_SHELL_SHELL_HIT_VEL;

	Colour m_Colour;
	// NOTE: Moving is true when this shell is sliding on the ground, and animating
	bool m_IsMoving = false;
	int m_DirMoving;

	// NOTE: This is true when this shell has been thrown into the air by the player
	// and hasn't hit the ground yet
	bool m_IsBouncing = false;
	bool m_IsFallingOffScreen = false;
	bool m_ShouldBeRemoved = false;
};
