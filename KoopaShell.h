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
	void Kick(int facingDir, bool wasThrown);
	void ShellHit();
	void Stomp();

	bool IsMoving();
private:
	static const double KICK_VEL;
	static const double SHELL_HIT_VEL;

	COLOUR m_Colour;
	bool m_IsMoving = false;
	int m_DirMoving;

	bool m_IsFalling = false;

	bool m_ShouldBeRemoved = false;
};
