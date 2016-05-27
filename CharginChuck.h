#pragma once

#include "Enemy.h"

struct INT2;

class CharginChuck : public Enemy
{
public:
	enum class AnimationState
	{
		WAITING, CHARGIN, HURT, JUMPING, DEAD
	};

	CharginChuck(DOUBLE2 startingPos, Level* levelPtr);
	virtual ~CharginChuck();

	CharginChuck(const CharginChuck&) = delete;
	CharginChuck&operator=(const CharginChuck&) = delete;

	void Tick(double deltaTime);
	void Paint();

	int GetWidth();
	int GetHeight();

	void HeadBonk();

	bool IsRising();
	AnimationState GetAnimationState();

private:
	void UpdateVelocity(double deltaTime);
	void CalculateNewTarget();
	INT2 GetAnimationFrame();
	void TickAnimations(double deltaTime);
	bool CalculateOnGround();

	bool TakeDamage(); // Returns whether or not we are still alive
	void TurnAround();
	void Jump(double deltaTime);

	static const int WIDTH = 20;
	static const int HEIGHT = 20;
	
	static const int MINIMUM_PLAYER_DISTANCE; // How close the player needs to get for us to see them
	static const double TARGET_OVERSHOOT_DISTANCE;
	static const double RUN_VEL;
	static const double JUMP_VEL;

	AnimationState m_AnimationState;
	CountdownTimer m_WaitingTimer;
	CountdownTimer m_HurtTimer;

	int m_HitsRemaining; // How many more hits from the player we can take before kicking the can

	double m_TargetX;

	bool m_ShouldRemoveActor;

};

