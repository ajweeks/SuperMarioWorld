#pragma once

#include "Enumerations.h"
#include "SpriteSheet.h"
#include "Item.h"
#include "CountdownTimer.h"
#include "INT2.h"

class Level;
class GameState;
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

	Player(Level* levelPtr, GameState* gameStatePtr);
	virtual ~Player();

	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

	void Tick(double deltaTime);
	void TickAnimations(double deltaTime);
	void Paint();
	void Reset();
	
	static PowerupState StringToPowerupState(std::string powerupStateStr);
	static std::string PowerupStateToString(PowerupState powerupState);

	// If no particle is neccessary, leave particlePosition as DOUBLE2()
	void AddScore(int score, bool combo, DOUBLE2 particlePosition = DOUBLE2());
	void AddRedStars(int numberOfRedStars);
	void AddCoin(Coin* coinPtr, bool playSound = true, bool showParticle = true);
	int GetScore();

	int GetLives();
	int GetCoinsCollected();
	int GetDragonCoinsCollected();
	int GetStarsCollected();
	Item::Type GetExtraItemType();

	void AddItemToBeHeld(Item* itemPtr);
	bool IsHoldingItem();
	Item* GetHeldItemPtr();
	void DropHeldItem();

	DOUBLE2 GetLinearVelocity();
	void SetLinearVelocity(const DOUBLE2& newLinearVelRef);
	DOUBLE2 GetPosition();
	int GetDirectionFacing();

	PowerupState GetPowerupState();

	bool IsOnGround();

	bool IsRidingYoshi();
	void RideYoshi(Yoshi* yoshiPtr);
	void DismountYoshi(bool runWild = false);

	// Called when the player bounces off an enemy's head, or off a moving koopa shell
	void Bounce();
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
	void SetClimbingBeanstalk(bool climbing);
	void SetOverlappingWithBeanstalk(bool overlapping);
	AnimationState GetAnimationState();
	int GetWidth();
	int GetHeight();
	bool IsDead();
	bool IsRunning();
	bool IsDucking();
	bool IsInvincible();
	bool IsAirborne();

	// NOTE: This should be called every time the player stomps on a KoopaTroopa's head
	//       Every frames m_FramesUntilEnemyHeadBounceEndSound is decremented, and plays the
	//		 sound when it reaches 0
	void ResetNumberOfFramesUntilEndStompSound();
	bool IsTransitioningPowerups();

private:
	void HandleKeyboardInput(double deltaTime);
	void HandleClimbingStateKeyboardInput(double deltaTime);
	void HandleYoshiKeyboardInput(double deltaTime);

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
	
	SpriteSheet* m_SpriteSheetPtr;

	static const int WALK_BASE_VEL;
	static const int RUN_BASE_VEL;
	static const int JUMP_VEL;
	static const int BOUNCE_VEL;
	static const double DEFAULT_GRAVITY;
	static const int STARTING_LIVES;
	static const int YOSHI_DISMOUNT_XVEL;
	static const int YOSHI_TURN_AROUND_FRAMES;
	static const double MARIO_SECONDS_PER_FRAME;

	bool m_IsOnGround;
	bool m_WasOnGround;
	int m_FramesSpentInAir;

	int m_Lives;
	int m_Coins;
	int m_DragonCoins;
	int m_Stars;
	int m_Score;
	int m_LastScoreAdded; // Used for calculating combo scores
	bool m_NeedsNewFixture;
	bool m_IsInvincible;

	CountdownTimer m_DeathAnimationTimer;
	CountdownTimer m_ChangingDirectionsTimer;
	CountdownTimer m_ShellKickAnimationTimer;
	CountdownTimer m_HeadStompSoundDelayTimer;
	CountdownTimer m_PowerupTransitionTimer;
	CountdownTimer m_InvincibilityTimer;
	CountdownTimer m_SpawnDustCloudTimer;
	CountdownTimer m_EnteringPipeTimer;
	CountdownTimer m_ExitingPipeTimer;
	CountdownTimer m_ScoreAddedTimer;

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

	std::vector<GrabBlock*> m_RecentlyTouchedGrabBlocksPtrArr;
};
