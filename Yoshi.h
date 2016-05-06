#pragma once

#include "Entity.h"
#include "Item.h"
#include "CountdownTimer.h"

class Player;
class Enemy;

class Yoshi : public Entity
{
public:
	enum class ANIMATION_STATE
	{
		EGG, BABY, WAITING, WALKING, WILD, JUMPING, FALLING
	};

	Yoshi(DOUBLE2 position, Level* levelPtr);
	virtual ~Yoshi();

	Yoshi(const Yoshi&) = delete;
	Yoshi&operator=(const Yoshi&) = delete;

	void Tick(double deltaTime);
	void Paint();

	int GetWidth();
	int GetHeight();

	bool IsHatching();
	void SetCarryingPlayer(bool carryingPlayer, Player* playerPtr = nullptr);

	int GetDirectionFacing();

private:
	void PaintAnimationFrame(double left, double top);
	void HandleKeyboardInput(double deltaTime);
	bool CalculateOnGround();
	std::string AnimationStateToString();

	static const int JUMP_VEL;
	static const int WALK_BASE_VEL;
	static const int RUN_BASE_VEL;
	static const int TOUNGE_VEL;

	static const int WIDTH = 12;
	static const int HEIGHT = 16;

	ANIMATION_STATE m_AnimationState;

	static const float HATCHING_SECONDS_PER_FRAME;
	static const float WAITING_SECONDS_PER_FRAME;
	static const float WALKING_SECONDS_PER_FRAME;

	// NOTE: Yoshi enters this state when the player takes damage while riding yoshi and is forced to dismount
	bool m_IsRunning = false;
	bool m_IsCarryingPlayer = false;
	bool m_IsToungeStuckOut = false;

	bool m_IsOnGround = false;

	CountdownTimer m_HatchingTimer;
	CountdownTimer m_ChangingDirectionsTimer;
	CountdownTimer m_ToungeTimer; // Records how long yoshi's tounge has been stuck out for
	SpriteSheet* m_SpriteSheetPtr = nullptr;

	PhysicsActor* m_ActToungePtr = nullptr;

	Player* m_PlayerPtr = nullptr;

	int m_DirFacingLastFrame;
	int m_DirFacing;
};
