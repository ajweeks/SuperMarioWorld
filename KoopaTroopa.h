#pragma once

#include "Enemy.h"
#include "Enumerations.h"
#include "Level.h"
#include "INT2.h"

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

	*/
	enum AnimationState
	{
		WALKING, SHELLESS, WALKING_SHELLESS, SQUASHED, UPSIDEDOWN_SHELLESS
	};

	KoopaTroopa(DOUBLE2 startingPos, Level* levelPtr, Colour colour);
	virtual ~KoopaTroopa();

	KoopaTroopa(const KoopaTroopa&) = delete;
	KoopaTroopa& operator=(const KoopaTroopa&) = delete;

	void Tick(double deltaTime);
	void Paint();

	int GetWidth() const;
	int GetHeight() const;

	AnimationState GetAnimationState() const;

	// Called when the player regular jumps on this koopa's head. (not spin jump)
	// This causes the koopa to lose its shell, or if it 
	// already has no shell then to die
	void HeadBonk();

	// Called when the player spin jumps on this koopa's head
	// Spawns a cloud paricle
	void StompKill();

	// Called when this koopa is hit by a shell
	// This kills the koopa and spawns a shell with a vertical velocity
	// that is removed once it reaches the bottom of the screen
	void ShellHit(bool shellWasBeingHeld = false);

	bool IsShelless() const;

	Colour GetColour() const;

private:
	static const int WIDTH = 14;
	static const int HEIGHT = 14;

	static const int WALK_VEL = 35;

	void ChangeAnimationState(AnimationState newAnimationState);
	INT2 DetermineAnimationFrame();
	
	void ChangeDirections();

	Colour m_Colour;
	AnimationState m_AnimationState;

	static const int FRAMES_OF_BEING_SHELLESS = 85;
	int m_FramesSpentBeingShelless = -1;

	static const int FRAMES_OF_TURNAROUND = 8;
	int m_FramesSpentTurningAround = -1;

	static const int FRAMES_OF_BEING_SQUASHED = 4;
	int m_FramesSpentBeingSquashed = -1;

	bool m_ShouldBeRemoved = false;

	bool m_ShouldAddKoopaShell = false;
	bool m_ShouldAddMovingUpwardKoopaShell = false;
};
