#pragma once

#include "Enumerations.h"
#include "Item.h"
#include "SMWTimer.h"
#include "INT2.h"

class Level;
class GameState;
class SpriteSheet;
class Entity;
class KoopaShell;
class Yoshi;
class GrabBlock;
class Pipe;
class Coin;
class DragonCoin;

class Player : public Entity
{
public:
	enum class PowerupState
	{
		NORMAL, SUPER, FIRE, CAPE, BALLOON, STAR, NONE
	};
	enum class AnimationState
	{
		WAITING, WALKING, JUMPING, SPIN_JUMPING, FALLING, CLIMBING, IN_PIPE, NONE
	};

	Player(Level* levelPtr, GameState* gameStatePtr, SessionInfo sessionInfo = {});
	virtual ~Player();

	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

	void Tick(double deltaTime);
	void Paint();
	void Reset();
	
	static PowerupState StringToPowerupState(const std::string& powerupStateStr);
	static std::string PowerupStateToString(PowerupState powerupState);

	// If no particle is neccessary, leave particlePosition as DOUBLE2()
	void AddScore(int score, bool combo, DOUBLE2 particlePosition = DOUBLE2());
	void AddRedStars(int numberOfRedStars);
	void AddCoin(Coin* coinPtr, bool playSound = true, bool showParticle = true);
	int GetScore() const;

	int GetLives() const;
	int GetCoinsCollected() const;
	int GetDragonCoinsCollected() const;
	int GetRedStarsCollected() const;
	Item::Type GetExtraItemType() const;

	void AddItemToBeHeld(Item* itemPtr);
	bool IsHoldingItem() const;
	Item* GetHeldItemPtr();
	void DropHeldItem();

	DOUBLE2 GetLinearVelocity() const;
	void SetLinearVelocity(const DOUBLE2& newLinearVelRef);
	DOUBLE2 GetPosition() const;
	int GetDirectionFacing() const;

	void TurnInvincible();
	// Called when the player bounces off an enemy's head, or off a moving koopa shell
	void Bounce(bool horizontallyAlso = true, int horizontalDir = 0);
	void Die();
	void TakeDamage();
	void KickShell(KoopaShell* koopaShellPtr, bool wasThrown);
	
	void ReleaseExtraItem(DOUBLE2 position);
	void OnItemPickup(Item* itemPtr);

	void SetTouchingGrabBlock(bool touching, GrabBlock* grabBlockPtr);
	void SetTouchingPipe(bool touching, Pipe* pipePtr = nullptr);
	void SetExitingPipe(Pipe* pipePtr);
	void SetPosition(DOUBLE2 newPosition);

	void MidwayGatePasshrough();
	AnimationState GetAnimationState();
	PowerupState GetPowerupState() const;
	void SetClimbingBeanstalk(bool climbing);
	void SetOverlappingWithBeanstalk(bool overlapping);
	int GetWidth() const;
	int GetHeight() const;
	bool IsDead() const;
	bool IsRunButtonHeldDown() const;
	bool IsSprinting() const; // Running at full speed
	bool IsDucking() const;
	bool IsInvincible() const;
	bool IsAirborne() const;
	bool IsOnGround() const;
	bool IsTransitioningPowerups() const;
	bool IsRidingYoshi() const;
	void RideYoshi(Yoshi* yoshiPtr);
	void DismountYoshi(bool runWild = false, double horizontalVel = -1.0);
	bool DEBUGIsTouchingGrabBlock() const;

	// NOTE: This should be called every time the player stomps on a KoopaTroopa's head
	//       Every frames m_FramesUntilEnemyHeadBounceEndSound is decremented, and plays the
	//		 sound when it reaches 0
	void ResetNumberOfFramesUntilEndStompSound();

private:
	void TickAnimations(double deltaTime);
	void HandleKeyboardInput(double deltaTime);
	void HandleClimbingStateKeyboardInput(double deltaTime);

	void AddDragonCoin(DragonCoin* dragonCoinPtr);
	void AddLife(DOUBLE2 particlePos);
	void KickHeldItem(double deltaTime, bool gently = false);
	void ChangePowerupState(PowerupState newPowerupState, bool isUpgrade = true);
	void EnterPipe();

	INT2 CalculateAnimationFrame();
	bool CalculateOnGround();
	SpriteSheet* GetSpriteSheetForPowerupState(PowerupState powerupState);
	String AnimationStateToString(AnimationState state);

	// Returns whether or not we were successful in entering a pipe
	bool AttemptToEnterPipes(); 

	static const double FRICTION;
	static const int WALKING_ACCELERATION;
	static const int RUNNING_ACCELERATION;
	static const int MAX_WALK_VEL;
	static const int MAX_RUN_VEL;
	static const int JUMP_VEL;
	static const int BEANSTALK_JUMP_VEL;
	static const int MAX_FALL_VEL;
	static const int BOUNCE_VEL;
	static const int STARTING_LIVES;
	static const int BOUNCE_VEL_HORIZONTAL;
	static const int YOSHI_DISMOUNT_XVEL;
	static const int YOSHI_TURN_AROUND_FRAMES;
	static const int DEATH_Y_VEL;

	static const double MARIO_SECONDS_PER_FRAME;
	static const double MARIO_SPINNING_SECONDS_PER_FRAME;

	bool m_IsOnGround;
	bool m_WasOnGround;
	int m_FramesSpentInAir;

	int m_Lives;
	int m_Coins;
	int m_DragonCoins;
	int m_RedStars;
	int m_Score;
	int m_LastScoreAdded; // Used for calculating combo scores
	bool m_NeedsNewFixture;
	bool m_IsInvincible;

	SMWTimer m_DeathAnimationTimer;
	SMWTimer m_ChangingDirectionsTimer;
	SMWTimer m_ItemKickAnimationTimer;
	SMWTimer m_HeadStompSoundDelayTimer;
	SMWTimer m_PowerupTransitionTimer;
	SMWTimer m_InvincibilityTimer;
	SMWTimer m_SpawnDustCloudTimer;
	SMWTimer m_EnteringPipeTimer;
	SMWTimer m_ExitingPipeTimer;
	SMWTimer m_ScoreAddedTimer;

	GameState* m_GameStatePtr;

	Item::Type m_ExtraItemToBeSpawnedType;
	Item* m_ExtraItemPtr; // This is the extra item slot mario has at the top of the screen

	Item* m_ItemHoldingPtr;

	int m_DirFacingLastFrame;
	int m_DirFacing;

	PowerupState m_PowerupState;
	PowerupState m_PrevPowerupState; // This is used to transition between states upon state change

	AnimationState m_AnimationState;
	bool m_IsDucking;
	bool m_IsLookingUp;
	bool m_IsRunning;
	bool m_IsHoldingItem;
	bool m_IsDead;
	bool m_IsRidingYoshi;

	bool m_IsOverlappingWithBeanstalk;
	static const int FRAMES_OF_CLIMBING_ANIMATION = 6;
	int m_FramesClimbingSinceLastFlip;
	int m_LastClimbingPose; // Flips between left and right as the player climbs

	Yoshi* m_RidingYoshiPtr;
	Pipe* m_PipeTouchingPtr;
	bool m_TouchingPipe;

	// We can be touching two grab blocks at once
	// Our most recently touched block will always be in index 0
	std::vector<GrabBlock*> m_RecentlyTouchedGrabBlocksPtrArr;
};
