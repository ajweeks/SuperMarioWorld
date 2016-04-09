#pragma once

#include "Enemy.h"
#include "Enumerations.h"
#include "Level.h"

class KoopaTroopa :	public Enemy
{
public:
	/*

	__Animation States__
	
	WALKING				Hit->  SHELLESS
	SHELLESS			Hit->  UPSIDEDOWN_SHELLESS
	WALKING_SHELLESS	Hit->  SQUASHED

	WALKING				SpinHit->  Spawn POOF particle
	SHELLESS			SpinHit->  Spawn POOF particle
	WALKING_SHELLESS	SpinHit->  Spawn POOF particle

	WALKING				ShellHit->  Spawn UPSIDEDOWN_SHELL
	WALKING_SHELLESS	ShellHit->  UPSIDEDOWN_SHELLESS
	SHELLESS			ShellHit->  UPSIDEDOWN_SHELLESS


	// SHELL ITEM:
	SHELL				ShellHit->  Spawn UPSIDEDOWN_SHELL

	*/
	enum ANIMATION_STATE
	{
		WALKING, SHELLESS, WALKING_SHELLESS, SQUASHED, UPSIDEDOWN_SHELLESS
	};

	KoopaTroopa(DOUBLE2& startingPos, Level* levelPtr, COLOUR colour);
	virtual ~KoopaTroopa();

	void Tick(double deltaTime);
	void Paint();

	int GetWidth();
	int GetHeight();

	ANIMATION_STATE GetAnimationState();

	// Called when the player hits this koopa on the head.
	// This causes the koopa to lose its shell, or if it 
	// already has no shell then it kill it.
	void Hit();
	// Called when this koopa is being stomped on, or being
	// hit by a shell
	void Die();

private:
	static const int WIDTH = 16;

	static const int WALK_VEL = 35;

	void ChangeAnimationState(ANIMATION_STATE newAnimationState);
	DOUBLE2 DetermineAnimationFrame();
	
	void ChangeDirections();

	COLOUR m_Color;
	ANIMATION_STATE m_AnimationState;
	static const int FRAMES_OF_TURNAROUND = 8;
	int m_FramesSpentTurningAround = -1;
	int m_CollisionBits = Level::COLLIDE_WITH_LEVEL | Level::COLLIDE_WITH_ENEMIES;

	bool m_ShouldBeRemoved = false;
	bool m_NeedsNewFixture = false;
};

