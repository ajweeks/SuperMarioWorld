#pragma once

#include "Enumerations.h"
#include "SpriteSheet.h"
#include "Item.h"

class Level;
class Entity;
class KoopaShell;
class Yoshi;

class Player : public Entity
{
public:
	enum class POWERUP_STATE
	{
		NORMAL, SUPER, FIRE, CAPE, BALLOON, STAR
	};
	enum class ANIMATION_STATE
	{
		WAITING, WALKING, JUMPING, SPIN_JUMPING, FALLING, CLIMBING, RIDING_YOSHI
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

	void AddItemToBeHeld(Item* itemPtr);
	bool IsHoldingItem();

	DOUBLE2 GetLinearVelocity();
	void SetLinearVelocity(const DOUBLE2& newLinearVelRef);
	DOUBLE2 GetPosition();
	int GetDirectionFacing();

	bool IsOnGround();

	void RideYoshi(Yoshi* yoshiPtr);

	void KickShell(KoopaShell* koopaShellPtr);

	// Called when the player bounces off an enemy's head
	void Bounce();

	void Die();
	void TakeDamage();

	void ReleaseExtraItem(DOUBLE2 position);

	void OnItemPickup(Item* itemPtr, Level* levelPtr);

	void SetClimbingBeanstalk(bool climbing);

	int GetWidth();
	int GetHeight();

	ANIMATION_STATE GetAnimationState();
	bool IsDead();
	bool IsRunning();

	void MidwayGatePasshrough();

	// NOTE: This should be called every time the player stomps on a KoopaTroopa's head
	//       Every frames m_FramesUntilEnemyHeadBounceEndSound is decremented, and plays the
	//		 sound when it reaches 0
	void ResetNumberOfFramesUntilEndStompSound();

	void SetOverlappingWithBeanstalk(bool overlapping);

private:
	INT2 CalculateAnimationFrame();

	void HandleKeyboardInput(double deltaTime);
	void HandleClimbingState(double deltaTime);

	void AddCoin(Level* levelPtr, bool playSound = true);
	void AddDragonCoin(Level* levelPtr);
	void AddLife();

	bool CalculateOnGround();

	void KickHeldItem(bool gently = false);

	String AnimationStateToString(ANIMATION_STATE state);

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

	static const int STARTING_LIVES = 5;
	int m_Lives;
	int m_Coins;
	int m_DragonCoins;
	int m_Stars;
	int m_Score;
	bool m_NeedsNewFixture;

	CountdownTimer m_DeathAnimation;
	CountdownTimer m_ChangingDirections;
	CountdownTimer m_Invincibility;
	CountdownTimer m_ShellKickAnimation;
	CountdownTimer m_HeadStompSoundDelay;
	CountdownTimer m_PowerupTransition;

	POWERUP_STATE m_PrevPowerupState; // This is used to transition between states upon state change

	Item::TYPE m_ExtraItemToBeSpawnedType;
	Item* m_ExtraItemPtr; // This is the extra item slot mario has at the top of the screen

	Item* m_ItemHoldingPtr;

	DOUBLE2 m_VelLastFrame;
	int m_DirFacingLastFrame;
	int m_DirFacing;

	POWERUP_STATE m_PowerupState;
	
	ANIMATION_STATE m_AnimationState;
	bool m_IsDucking;
	bool m_IsLookingUp;
	bool m_IsRunning;
	bool m_IsHoldingItem;
	bool m_IsDead;

	bool m_IsOverlappingWithBeanstalk;

	Yoshi* m_RidingYoshiPtr;
};
