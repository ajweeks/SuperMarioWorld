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
	CharginChuck& operator=(const CharginChuck&) = delete;

	void Tick(double deltaTime);
	void Paint();

	int GetWidth() const;
	int GetHeight() const;

	void SetPaused(bool paused);

	void HeadBonk();

	bool IsRising() const;
	AnimationState GetAnimationState() const;

private:
	void UpdateVelocity(double deltaTime);
	void CalculateNewTarget();
	INT2 GetAnimationFrame();
	void TickAnimations(double deltaTime);
	bool CalculateOnGround();

	void SetAnimationState(AnimationState newAnimationState);

	void TakeDamage(); // Returns whether or not we are still alive
	void TurnAround();
	void Jump(double deltaTime);

	static const int WIDTH = 20;
	static const int HEIGHT = 20;
	
	static const double TARGET_OVERSHOOT_DISTANCE;
	static const double RUN_VEL;
	static const double JUMP_VEL;
	
	static const double CHARGIN_SECONDS_PER_FRAME;
	static const double SHAKING_HEAD_HURT_SECONDS_PER_FRAME;
	static const int FRAMES_OF_SHAKING_HEAD;
	static const double SITTING_HURT_SECONDS_PER_FRAME;
	static const int FRAMES_OF_SITTING;

	AnimationState m_AnimationState;

	SMWTimer m_WaitingTimer;
	SMWTimer m_HurtTimer;

	int m_HitsRemaining; // How many more hits from the player we can take before kicking the can

	double m_TargetX;

	bool m_ShouldRemoveActor;

};
