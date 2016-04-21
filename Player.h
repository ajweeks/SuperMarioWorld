#pragma once

#include "Enumerations.h"
#include "SpriteSheet.h"
#include "Item.h"

class Level;
class Entity;

class Player : public Entity
{
public:
	enum class POWERUP_STATE
	{
		NORMAL, SUPER, FIRE, CAPE, BALLOON, STAR
	};
	enum class ANIMATION_STATE
	{
		// NOTE: FAST_FALLING is when mario is running full speed and has a 
		// positive y vel, not faster falling downwards
		WAITING, WALKING, RUNNING, JUMPING, SPIN_JUMPING, DUCKING, LOOKING_UPWARDS, 
		FALLING, FAST_FALLING, DYING, CLIMBING, CHANGING_DIRECTIONS
	};

	Player(Level* levelPtr);
	virtual ~Player();

	Player& operator=(const Player&) = delete;
	Player(const Player&) = delete;

	void Tick(double deltaTime);
	void TickAnimations(double deltaTime);

	void Paint();

	void Reset();

	int GetLives();
	int GetCoinsCollected();
	int GetDragonCoinsCollected();
	int GetStarsCollected();
	int GetScore();
	Item::TYPE GetExtraItemType();

	DOUBLE2 GetLinearVelocity();
	void SetLinearVelocity(const DOUBLE2& newLinearVelRef);
	DOUBLE2 GetPosition();
	int GetDirectionFacing();

	bool IsOnGround();

	// Called when the player bounces off an enemy's head
	void Bounce();

	void Die();
	void TakeDamage();

	void OnItemPickup(Item* itemPtr, Level* levelPtr);

	int GetWidth();
	int GetHeight();

	ANIMATION_STATE GetAnimationState();

	// NOTE: This should be called every time the player stomps on a KoopaTroopa's head
	//       Every frames m_FramesUntilEnemyHeadBounceEndSound is decremented, and plays the
	//		 sound when it reaches 0
	void ResetNumberOfFramesUntilEndStompSound();

private:
	INT2 CalculateAnimationFrame();

	void HandleKeyboardInput(double deltaTime, Level* levelPtr);

	void AddCoin(Level* levelPtr, bool playSound = true);
	void AddDragonCoin(Level* levelPtr);
	void AddLife();

	bool CalculateOnGround();

	void ChangePowerupState(POWERUP_STATE newPowerupState, bool isUpgrade = true);
	SpriteSheet* GetSpriteSheetForPowerupState(POWERUP_STATE powerupState);

	SpriteSheet* m_SpriteSheetPtr = nullptr;

	static const double DEFAULT_GRAVITY;

	static const double WALK_SPEED;
	static const double RUN_SPEED;

	static const int BOUNCE_VEL = -195;

	bool m_IsOnGround;
	bool m_WasOnGround;
	int m_FramesSpentInAir;

	int m_Lives;
	int m_Coins;
	int m_DragonCoins;
	int m_Stars;
	int m_Score;
	bool m_NeedsNewFixture;

	static const int FRAMES_OF_DEATH_ANIMATION = 240;
	int m_FramesOfDeathAnimationElapsed;

	static const int TOTAL_FRAMES_OF_POWERUP_TRANSITION = 50;
	int m_FramesOfPowerupTransitionElapsed; // NOTE: Perhaps this variable name is a tad long...
	POWERUP_STATE m_PrevPowerupState; // This is used to transition between states upon state change

	// NOTE: If this is set to something other than NONE, then at the start of Tick() we will create 
	// a new item with this type and assign it to m_ExtraItemPtr
	Item::TYPE m_ExtraItemToBeSpawnedType;
	Item* m_ExtraItemPtr = nullptr; // This is the extra item slot mario has at the top of the screen

	String AnimationStateToString(ANIMATION_STATE state);

	int m_DirFacingLastFrame;
	int m_DirFacing;

	POWERUP_STATE m_PowerupState;
	ANIMATION_STATE m_AnimationState;

	static const int FRAMES_TO_WAIT_BEFORE_PLAYING_KOOPA_HEAD_STOMP_END_SOUND = 10;
	int m_FramesUntilEnemyHeadBounceEndSound = -1;
};
