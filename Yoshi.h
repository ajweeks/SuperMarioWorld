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
		EGG, BABY, WAITING, WILD, FALLING
	};

	Yoshi(DOUBLE2 position, Level* levelPtr);
	virtual ~Yoshi();

	Yoshi(const Yoshi&) = delete;
	Yoshi& operator=(const Yoshi&) = delete;

	void Tick(double deltaTime);
	void Paint();

	int GetWidth();
	int GetHeight();

	void EatItem(Item* itemPtr);
	void EatEnemy(Enemy* enemyPtr);

	bool IsHatching();
	void RunWild();
	void SetCarryingPlayer(bool carryingPlayer, Player* playerPtr = nullptr);

	int GetDirectionFacing();

	ANIMATION_STATE GetAnimationState();
	bool IsAirborne();
	void SpitOutItem();
	bool IsToungeStuckOut();
	void StickToungeOut(double deltaTime);

	void SetPaused(bool paused);

	const CountdownTimer GetToungeTimer();

	static const int JUMP_VEL; // Yoshi never uses this field directly, but the player class does while they are riding yoshi

	static const float HATCHING_SECONDS_PER_FRAME;
	static const float WAITING_SECONDS_PER_FRAME;
	static const float WALKING_SECONDS_PER_FRAME;
	static const float RUNNING_SECONDS_PER_FRAME;
	static const float TOUNGE_STUCK_OUT_SECONDS_PER_FRAME;

private:
	void PaintAnimationFrame(double left, double top);
	void HandleKeyboardInput(double deltaTime);
	bool CalculateOnGround();
	void UpdatePosition(double deltaTime); // Only called when yoshi is "running wild"

	void SwallowItem();
	
	std::string AnimationStateToString();

	static const int RUN_VEL;
	static const int TOUNGE_VEL;

	static const int MAX_ITEMS_EATEN = 9;

	static const int WIDTH = 12;
	static const int HEIGHT = 16;

	ANIMATION_STATE m_AnimationState;

	bool m_IsCarryingPlayer = false;
	bool m_IsToungeStuckOut = false;

	bool m_IsOnGround = false;
	bool m_WasOnGround = false;
	bool m_NeedsNewFixture = false;

	double m_ToungeXVel = 0.0;
	double m_ToungeLength = 0.0; // How far our tounge is stuck out

	CountdownTimer m_HatchingTimer;
	CountdownTimer m_ToungeTimer; // Records how long yoshi's tounge has been stuck out for
	
	SpriteSheet* m_SpriteSheetPtr = nullptr;
	PhysicsActor* m_ActToungePtr = nullptr;
	Player* m_PlayerPtr = nullptr;

	// NOTE: This is used only for items which are not immediately eaten by yoshi
	Item::TYPE m_ItemInMouthType;
	Item* m_ItemToBeRemovedPtr = nullptr;
	Enemy* m_EnemyToBeRemovedPtr = nullptr;

	int m_ItemsEaten;
	int m_DirFacing;
};
