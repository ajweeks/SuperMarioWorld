#include "stdafx.h"

#include "Player.h"
#include "Game.h"
#include "GameState.h"
#include "StateManager.h"
#include "Enumerations.h"
#include "SpriteSheetManager.h"
#include "INT2.h"
#include "GameSession.h"

#include "SoundManager.h"
#include "DustParticle.h"
#include "NumberParticle.h"
#include "SplatParticle.h"
#include "GrabBlock.h"

#include "SuperMushroom.h"
#include "Coin.h"
#include "DragonCoin.h"
#include "KoopaShell.h"
#include "Yoshi.h"
#include "Pipe.h"

#define ENTITY_MANAGER (EntityManager::GetInstance())

// STATIC INITIALIZATIONS
const double Player::DEFAULT_GRAVITY = 0.98;
const int Player::JUMP_VEL = -16000;
const int Player::WALK_BASE_VEL = 5000;
const int Player::RUN_BASE_VEL = 9500;
const int Player::BOUNCE_VEL = -195;
const int Player::STARTING_LIVES = 5;
const int Player::YOSHI_DISMOUNT_XVEL = 2500;
const double Player::MARIO_SECONDS_PER_FRAME = 0.065;

Player::Player(Level* levelPtr, GameState* gameStatePtr) :
	Entity(DOUBLE2(), BodyType::DYNAMIC, levelPtr, ActorId::PLAYER, this),
	m_GameStatePtr(gameStatePtr)
{
	Reset();

	m_Lives = STARTING_LIVES;
	m_AnimInfo.secondsPerFrame = MARIO_SECONDS_PER_FRAME;

	m_RecentlyTouchedGrabBlocksPtrArr = std::vector<GrabBlock*>(2);

	// TODO: Slow down the player's animation frame rate when he slows down
}

Player::~Player()
{
}

void Player::Reset()
{
	m_ActPtr->SetPosition(DOUBLE2(30, 341));
	m_ActPtr->SetLinearVelocity(DOUBLE2(0, 0));
	m_ActPtr->SetActive(true);
	m_ActPtr->SetSensor(false);

	m_IsOnGround = false;

	m_AnimInfo.Reset();

	m_Score = 0;
	m_Coins = 0;
	m_Stars = 0;
	m_DragonCoins = 0;

	m_SpriteSheetPtr = SpriteSheetManager::smallMarioPtr;
	m_NeedsNewFixture = true;

	m_ExtraItemPtr = nullptr;
	m_ExtraItemToBeSpawnedType = Item::TYPE::NONE;

	m_ItemHoldingPtr = nullptr;

	m_FramesSpentInAir = -1;

	m_DeathAnimationTimer = CountdownTimer(240);
	m_ShellKickAnimationTimer = CountdownTimer(10);
	m_HeadStompSoundDelayTimer = CountdownTimer(10);
	m_PowerupTransitionTimer = CountdownTimer(50);
	m_InvincibilityTimer = CountdownTimer(115);
	m_SpawnDustCloudTimer = CountdownTimer(4);
	m_ChangingDirectionsTimer = CountdownTimer(3);
	m_EnteringPipeTimer = CountdownTimer(60);
	m_ExitingPipeTimer = CountdownTimer(35);

	m_AnimationState = ANIMATION_STATE::WAITING;
	m_IsDucking = false;
	m_IsLookingUp = false;
	m_IsRunning = false;
	m_IsHoldingItem = false;
	m_IsDead = false;
	m_IsRidingYoshi = false;
	m_IsOverlappingWithBeanstalk = false;

	m_PowerupState = POWERUP_STATE::NORMAL;
	m_PrevPowerupState = POWERUP_STATE::NORMAL;
	m_DirFacing = Direction::RIGHT;
	m_DirFacingLastFrame = Direction::RIGHT;

	m_FramesClimbingSinceLastFlip = 0;
	m_LastClimbingPose = Direction::RIGHT;

	m_RidingYoshiPtr = nullptr;
	m_PipeTouchingPtr = nullptr;
	m_TouchingPipe = false;

	for (size_t i = 0; i < m_RecentlyTouchedGrabBlocksPtrArr.size(); ++i)
	{
		m_RecentlyTouchedGrabBlocksPtrArr[i] = nullptr;
	}
}

void Player::Tick(double deltaTime)
{
#ifdef SMW_ENABLE_JUMP_TO
	if (GAME_ENGINE->IsKeyboardKeyPressed('O'))
	{
		m_ActPtr->SetPosition(DOUBLE2(SMW_JUMP_TO_POS_X, 250));
	}
#endif
	
	if (m_IsDead)
	{
		if (m_DeathAnimationTimer.Tick() && m_DeathAnimationTimer.IsComplete())
		{
			m_LevelPtr->Reset();
			return;
		}

		if (m_DeathAnimationTimer.FramesElapsed() == 1)
		{
			m_LevelPtr->SetPaused(true);
		}

		// NOTE: The player doesn't fly upward until after a short delay
		// LATER: Move all harcoded values out somewhere else
		if (m_DeathAnimationTimer.FramesElapsed() == 40)
		{
			m_ActPtr->SetActive(true);
			m_ActPtr->SetLinearVelocity(DOUBLE2(0, -420));
		}
	}

	if (m_EnteringPipeTimer.Tick())
	{
		// Move 0.6 units per frame while in a pipe
		double delta = 0.6;
		double dx = 0.0, dy = 0.0;
		switch (m_PipeTouchingPtr->GetOrientation())
		{
		case Pipe::Orientation::LEFT:	dx = delta; break;
		case Pipe::Orientation::RIGHT:	dx = -delta; break;
		case Pipe::Orientation::UP:		dy = delta; break;
		case Pipe::Orientation::DOWN:	dy = -delta; break;
		}
		m_ActPtr->SetPosition(m_ActPtr->GetPosition() + DOUBLE2(dx, dy));

		if (m_EnteringPipeTimer.IsComplete())
		{
			SessionInfo currentSessionInfo;
			GameSession::RecordSessionInfo(currentSessionInfo, m_LevelPtr);
			
			if (m_LevelPtr->IsUnderground())
			{
				m_GameStatePtr->LeaveUnderground(currentSessionInfo, m_PipeTouchingPtr);
			}
			else
			{
				m_GameStatePtr->EnterUnderground(currentSessionInfo, m_PipeTouchingPtr);
			}
		}
		return;
	}
	if (m_ExitingPipeTimer.Tick())
	{
		double delta = 0.6;
		double dx = 0.0, dy = 0.0;
		switch (m_PipeTouchingPtr->GetOrientation())
		{
		case Pipe::Orientation::LEFT:	dx = -delta; break;
		case Pipe::Orientation::RIGHT:	dx =  delta; break;
		case Pipe::Orientation::UP:		dy = -delta; break;
		case Pipe::Orientation::DOWN:	dy =  delta; break;
		}
		m_ActPtr->SetPosition(m_ActPtr->GetPosition() + DOUBLE2(dx, dy));

		if (m_ExitingPipeTimer.IsComplete())
		{
			m_ActPtr->SetSensor(false);
			m_ActPtr->SetGravityScale(DEFAULT_GRAVITY);
			m_LevelPtr->SetPaused(false);
			m_AnimationState = ANIMATION_STATE::WAITING;
		}
		return;
	}

	if (m_HeadStompSoundDelayTimer.Tick() && m_HeadStompSoundDelayTimer.IsComplete())
	{
		SoundManager::PlaySoundEffect(SoundManager::SOUND::ENEMY_HEAD_STOMP_END);
	}

	m_ChangingDirectionsTimer.Tick();
	m_ShellKickAnimationTimer.Tick();
	m_SpawnDustCloudTimer.Tick();

	if (m_InvincibilityTimer.Tick() && m_InvincibilityTimer.IsComplete())
	{
		m_IsInvincible = false;
	}

	if (m_NeedsNewFixture)
	{
		double oldHalfHeight = GetHeight() / 2;

		b2Fixture* fixturePtr = m_ActPtr->GetBody()->GetFixtureList();
		while (fixturePtr != nullptr)
		{
			b2Fixture* nextFixturePtr = fixturePtr->GetNext();
			m_ActPtr->GetBody()->DestroyFixture(fixturePtr);
			fixturePtr = nextFixturePtr;
		}

		m_ActPtr->SetActive(true);
		m_ActPtr->AddBoxFixture(GetWidth(), GetHeight(), 0.0, 0.02);

		double newHalfHeight = GetHeight() / 2;
		double newCenterY = m_ActPtr->GetPosition().y + (newHalfHeight - oldHalfHeight);
		m_ActPtr->SetPosition(DOUBLE2(m_ActPtr->GetPosition().x, newCenterY));

		m_NeedsNewFixture = false;
	}

	HandleKeyboardInput(deltaTime);

	if (m_AnimationState != ANIMATION_STATE::CLIMBING &&
		m_IsOverlappingWithBeanstalk &&
		(GAME_ENGINE->IsKeyboardKeyDown(VK_UP) || 
		(!m_IsOnGround && GAME_ENGINE->IsKeyboardKeyDown(VK_DOWN))))
	{
		SetClimbingBeanstalk(true);
	}

	if (m_IsHoldingItem)
	{
		// NOTE: Once the player is no longer running, they should kick the shell
		if (!m_IsRunning)
		{
			// When ducking, the player gently releases the item
			KickHeldItem(m_IsDucking);
		}
		else
		{
			m_ItemHoldingPtr->Tick(deltaTime);
		}
	}

	TickAnimations(deltaTime);

	m_DirFacingLastFrame = m_DirFacing;
}

void Player::HandleClimbingStateKeyboardInput(double deltaTime)
{
	if (GAME_ENGINE->IsKeyboardKeyPressed('Z')) // NOTE: Regular jump
	{
		SetClimbingBeanstalk(false);
		m_AnimationState = ANIMATION_STATE::JUMPING;
		SoundManager::PlaySoundEffect(SoundManager::SOUND::PLAYER_JUMP);
		m_FramesSpentInAir = 0;
		// TODO: Move vine jump value out to a const int
		m_ActPtr->SetLinearVelocity(DOUBLE2(0, -21500 * deltaTime));
		return;
	}

	DOUBLE2 prevPlayerPos = m_ActPtr->GetPosition();
	bool moved = true;
	if (GAME_ENGINE->IsKeyboardKeyDown(VK_UP))
	{
		m_ActPtr->SetPosition(prevPlayerPos.x, prevPlayerPos.y - 1);
	}
	else if (GAME_ENGINE->IsKeyboardKeyDown(VK_DOWN))
	{
		m_ActPtr->SetPosition(prevPlayerPos.x, prevPlayerPos.y + 1);

		m_IsOnGround = CalculateOnGround();
		if (m_IsOnGround)
		{
			SetClimbingBeanstalk(false);
			return;
		}
	} 
	else if (GAME_ENGINE->IsKeyboardKeyDown(VK_LEFT))
	{
		m_ActPtr->SetPosition(prevPlayerPos.x - 1, prevPlayerPos.y);
	}
	else if (GAME_ENGINE->IsKeyboardKeyDown(VK_RIGHT))
	{
		m_ActPtr->SetPosition(prevPlayerPos.x + 1, prevPlayerPos.y);
	}
	else moved = false;

	--m_FramesClimbingSinceLastFlip;
	
	if (moved)
	{
		if (m_FramesClimbingSinceLastFlip < 0)
		{
			m_FramesClimbingSinceLastFlip = FRAMES_OF_CLIMBING_ANIMATION;
			m_LastClimbingPose = -m_LastClimbingPose;
		}
	}
}

void Player::HandleYoshiKeyboardInput(double deltaTime)
{
	assert(m_RidingYoshiPtr != nullptr);
	assert(m_IsRidingYoshi);


	if (AttemptToEnterPipes()) return;

	double horizontalVel = 0.0;
	double verticalVel = 0.0;

	if (GAME_ENGINE->IsKeyboardKeyPressed('X')) // Spin jump
	{
		verticalVel = JUMP_VEL * deltaTime;
		if (m_RidingYoshiPtr->IsAirborne())
		{
			m_AnimationState = ANIMATION_STATE::JUMPING;
		}
		else
		{
			m_AnimationState = ANIMATION_STATE::SPIN_JUMPING;
			m_DirFacing = -m_RidingYoshiPtr->GetDirectionFacing();
			horizontalVel = m_DirFacing * YOSHI_DISMOUNT_XVEL * deltaTime;
			SoundManager::PlaySoundEffect(SoundManager::SOUND::PLAYER_SPIN_JUMP);
		}
		m_FramesSpentInAir = 0;
		m_ActPtr->SetLinearVelocity(DOUBLE2(horizontalVel, verticalVel));

		DismountYoshi();
		return;
	}

	m_IsOnGround = CalculateOnGround();
	if (m_IsOnGround && GAME_ENGINE->IsKeyboardKeyDown(VK_DOWN))
	{
		m_IsDucking = true;
		m_AnimationState = ANIMATION_STATE::WAITING;
	}
	else
	{
		m_IsDucking = false;
	}

	if (!m_WasOnGround && m_IsOnGround)
	{
		m_AnimationState = ANIMATION_STATE::WAITING;
	}

	if (m_IsDucking == false)
	{
		if (GAME_ENGINE->IsKeyboardKeyDown(VK_LEFT))
		{
			m_AnimationState = ANIMATION_STATE::WALKING;
			m_DirFacing = Direction::LEFT;
			horizontalVel = WALK_BASE_VEL * deltaTime;
		}
		else if (GAME_ENGINE->IsKeyboardKeyDown(VK_RIGHT))
		{
			m_AnimationState = ANIMATION_STATE::WALKING;
			m_DirFacing = Direction::RIGHT;
			horizontalVel = WALK_BASE_VEL * deltaTime;
		}
		else if (m_AnimationState == ANIMATION_STATE::WALKING)
		{
			m_AnimationState = ANIMATION_STATE::WAITING;
		}

		if (GAME_ENGINE->IsKeyboardKeyPressed('A') ||
			GAME_ENGINE->IsKeyboardKeyPressed('S'))
		{
			m_RidingYoshiPtr->SpitOutItem();
			
			if (m_RidingYoshiPtr->IsTongueStuckOut() == false)
			{
				m_RidingYoshiPtr->StickTongueOut(deltaTime);
			}
		}

		if (GAME_ENGINE->IsKeyboardKeyDown('A') ||
			GAME_ENGINE->IsKeyboardKeyDown('S'))
		{
			if (m_AnimationState == ANIMATION_STATE::WALKING)
			{
				m_IsRunning = true;
				horizontalVel = RUN_BASE_VEL * deltaTime;
			}
		}
		else
		{
			m_IsRunning = false;
		}
	}

	if (m_IsOnGround && GAME_ENGINE->IsKeyboardKeyPressed('Z'))
	{
		m_AnimationState = ANIMATION_STATE::JUMPING;
		verticalVel = m_RidingYoshiPtr->JUMP_VEL * deltaTime;
	}

	DOUBLE2 oldVel = m_ActPtr->GetLinearVelocity();
	DOUBLE2 newVel = oldVel;

	if (horizontalVel != 0.0)
	{
		if (m_DirFacing == Direction::RIGHT) newVel.x = horizontalVel;
		else newVel.x = -horizontalVel;
	}
	if (verticalVel != 0.0)
	{
		newVel.y = verticalVel;
	}

	if (m_DirFacingLastFrame != m_DirFacing)
	{
		m_ChangingDirectionsTimer.Start();

		if (m_SpawnDustCloudTimer.IsActive() == false && m_IsOnGround)
		{
			DustParticle* dustParticlePtr = new DustParticle(m_ActPtr->GetPosition() + DOUBLE2(0, GetHeight() / 2 + 1));
			m_LevelPtr->AddParticle(dustParticlePtr);
			m_SpawnDustCloudTimer.Start();
		}
	}

	m_ActPtr->SetLinearVelocity(newVel);

	m_DirFacingLastFrame = m_DirFacing;
	m_WasOnGround = m_IsOnGround;
}

void Player::HandleKeyboardInput(double deltaTime)
{
	if (m_IsDead)
	{
		return; // NOTE: The player can't do much now...
	}

	if (m_AnimationState == ANIMATION_STATE::CLIMBING)
	{
		HandleClimbingStateKeyboardInput(deltaTime);
		return;
	}
	else if (m_IsRidingYoshi)
	{
		HandleYoshiKeyboardInput(deltaTime);
		return;
	}

	
	if (AttemptToEnterPipes()) return;

	double verticalVel = 0.0;
	double horizontalVel = 0.0;
	if (m_IsDucking &&
		GAME_ENGINE->IsKeyboardKeyDown(VK_DOWN) == false)
	{
		m_AnimationState = ANIMATION_STATE::WAITING;
		m_IsDucking = false;
	}
	if (m_IsLookingUp &&
		GAME_ENGINE->IsKeyboardKeyDown(VK_UP) == false)
	{
		m_AnimationState = ANIMATION_STATE::WAITING;
		m_IsLookingUp = false;
	}

	m_IsOnGround = CalculateOnGround();
	if (m_IsOnGround)
	{
		if (m_WasOnGround == false)
		{
			m_AnimationState = ANIMATION_STATE::WAITING;
		}

		bool regularJumpKeyPressed = GAME_ENGINE->IsKeyboardKeyPressed('Z');
		bool spinJumpKeyPressed = GAME_ENGINE->IsKeyboardKeyPressed('X');
		if (m_AnimationState != ANIMATION_STATE::SPIN_JUMPING &&
			regularJumpKeyPressed ||
			(spinJumpKeyPressed && m_IsHoldingItem) ||
			(spinJumpKeyPressed && m_IsRidingYoshi)) // NOTE: Regular jump
		{
			m_AnimationState = ANIMATION_STATE::JUMPING;
			m_IsOnGround = false;
			SoundManager::PlaySoundEffect(SoundManager::SOUND::PLAYER_JUMP);
			m_FramesSpentInAir = 0;
			verticalVel = JUMP_VEL * deltaTime;
		}
		else if (spinJumpKeyPressed) // NOTE: Spin jump
		{
			m_AnimationState = ANIMATION_STATE::SPIN_JUMPING;
			m_IsOnGround = false;
			SoundManager::PlaySoundEffect(SoundManager::SOUND::PLAYER_SPIN_JUMP);
			m_FramesSpentInAir = 0;
			verticalVel = JUMP_VEL * deltaTime;
		}
		else
		{
			m_FramesSpentInAir = -1;
		}
	}
	else
	{
		m_FramesSpentInAir++;

		// NOTE: The player is still rising and can hold down the jump key to jump higher
		if (m_ActPtr->GetLinearVelocity().y < -100.0)
		{
			if (GAME_ENGINE->IsKeyboardKeyDown('Z') ||
				GAME_ENGINE->IsKeyboardKeyDown('X'))
			{
				// NOTE: gravityScale is close to 1 at the start of the jump
				// and goes towards 0 near the apex
				double gravityScale = (m_FramesSpentInAir / 12.0) * 0.5;

				// NOTE: This ensures gravityScale is in the range [0, 0.98]
				gravityScale = max(0.0, min(DEFAULT_GRAVITY, gravityScale));

				m_ActPtr->SetGravityScale(gravityScale);
			}
			else
			{
				m_ActPtr->SetGravityScale(DEFAULT_GRAVITY);
			}
		}
		else
		{
			m_ActPtr->SetGravityScale(DEFAULT_GRAVITY);
			if (m_AnimationState != ANIMATION_STATE::SPIN_JUMPING)
			{
				m_AnimationState = ANIMATION_STATE::FALLING;
			}
		}
	}

	if (GAME_ENGINE->IsKeyboardKeyDown(VK_LEFT))
	{
		m_DirFacing = Direction::LEFT;
		if (m_AnimationState != ANIMATION_STATE::JUMPING &&
			m_AnimationState != ANIMATION_STATE::SPIN_JUMPING &&
			m_AnimationState != ANIMATION_STATE::FALLING &&
			!m_IsDucking &&
			!m_IsRunning)
		{
			m_AnimationState = ANIMATION_STATE::WALKING;
		}
		horizontalVel = WALK_BASE_VEL * deltaTime;
	}
	else if (GAME_ENGINE->IsKeyboardKeyDown(VK_RIGHT))
	{
		m_DirFacing = Direction::RIGHT;
		if (m_AnimationState != ANIMATION_STATE::JUMPING &&
			m_AnimationState != ANIMATION_STATE::SPIN_JUMPING &&
			m_AnimationState != ANIMATION_STATE::FALLING &&
			!m_IsDucking &&
			!m_IsRunning)
		{
			m_AnimationState = ANIMATION_STATE::WALKING;
		}
		horizontalVel = WALK_BASE_VEL * deltaTime;
	}

	// Grab blocks
	if (m_IsHoldingItem == false &&
		GAME_ENGINE->IsKeyboardKeyPressed('A') || GAME_ENGINE->IsKeyboardKeyPressed('S'))
	{
		if (m_RecentlyTouchedGrabBlocksPtrArr[0] != nullptr)
		{
			AddItemToBeHeld((Item*)m_RecentlyTouchedGrabBlocksPtrArr[0]);
			m_RecentlyTouchedGrabBlocksPtrArr[0] = nullptr;

			if (m_RecentlyTouchedGrabBlocksPtrArr[1] != nullptr)
			{
				m_RecentlyTouchedGrabBlocksPtrArr[0] = m_RecentlyTouchedGrabBlocksPtrArr[1];
				m_RecentlyTouchedGrabBlocksPtrArr[1] = nullptr;
			}
		}
	}

	if (horizontalVel != 0.0)
	{
		// TODO: Move all keybindings out to external file
		if (GAME_ENGINE->IsKeyboardKeyDown('A') || GAME_ENGINE->IsKeyboardKeyDown('S')) // Running
		{
			if (!m_IsDucking)
			{
				if (m_IsOnGround &&
					m_AnimationState != ANIMATION_STATE::SPIN_JUMPING &&
					m_AnimationState != ANIMATION_STATE::JUMPING)
				{
					m_AnimationState = ANIMATION_STATE::WALKING;
					m_IsRunning = true;
				}
				horizontalVel = RUN_BASE_VEL * deltaTime;
			}
		}
		else
		{
			m_IsRunning = false;
		}
	}
	else
	{
		if (!GAME_ENGINE->IsKeyboardKeyDown('A') && !GAME_ENGINE->IsKeyboardKeyDown('S')) // Running
		{
			m_IsRunning = false;
		}
	}

	// Only look up if we aren't moving horizontally or vertically
	if (GAME_ENGINE->IsKeyboardKeyDown(VK_UP))
	{
		if (m_IsOverlappingWithBeanstalk)
		{
			SetClimbingBeanstalk(true);
			return;
		}
		else if (abs(m_ActPtr->GetLinearVelocity().x) < 0.01 &&
			abs(m_ActPtr->GetLinearVelocity().y) < 0.01)
		{
			m_AnimationState = ANIMATION_STATE::WAITING;
			m_IsLookingUp = true;
		}
	}
	else if (GAME_ENGINE->IsKeyboardKeyDown(VK_DOWN))
	{
		if (m_IsOverlappingWithBeanstalk && !m_IsOnGround)
		{
			SetClimbingBeanstalk(true);
			return;
		}
		else if (m_IsOnGround)
		{
			m_AnimationState = ANIMATION_STATE::WAITING;
			horizontalVel = 0.0;
		}

		m_IsDucking = true;

	}

	if (horizontalVel == 0.0 &&
		verticalVel == 0.0 && 
		abs(m_ActPtr->GetLinearVelocity().x) < 0.001 &&
		abs(m_ActPtr->GetLinearVelocity().y) < 0.001)
	{
		if (!m_IsDucking &&
			!m_IsLookingUp &&
			m_AnimationState != ANIMATION_STATE::JUMPING &&
			m_AnimationState != ANIMATION_STATE::SPIN_JUMPING)
		{
			m_AnimationState = ANIMATION_STATE::WAITING;
		}
	}

	DOUBLE2 oldVel = m_ActPtr->GetLinearVelocity();
	DOUBLE2 newVel = oldVel;
	if (horizontalVel != 0.0) 
	{
		if (m_DirFacing == Direction::LEFT) horizontalVel = -horizontalVel;
		newVel.x = horizontalVel;
	}
	if (verticalVel != 0.0) 
	{
		newVel.y = verticalVel;
	}

	if (m_DirFacingLastFrame != m_DirFacing)
	{
		m_ChangingDirectionsTimer.Start();

		if (m_SpawnDustCloudTimer.IsActive() == false && m_IsOnGround)
		{
			DustParticle* dustParticlePtr = new DustParticle(m_ActPtr->GetPosition() + DOUBLE2(0, GetHeight() / 2 + 1));
			m_LevelPtr->AddParticle(dustParticlePtr);
			m_SpawnDustCloudTimer.Start();
		}
	}

	double maxHorizontalAcceleration = 14;
	if (abs(oldVel.x - newVel.x) > maxHorizontalAcceleration)
	{
		if (newVel.x > oldVel.x)
		{
			newVel.x = oldVel.x + maxHorizontalAcceleration;
		}
		else
		{
			newVel.x = oldVel.x - maxHorizontalAcceleration;
		}
	}

	m_ActPtr->SetLinearVelocity(newVel);

	// NOTE: Prevents the player from walking off the side of the level
	if (m_ActPtr->GetPosition().x < GetWidth())
	{
		m_ActPtr->SetLinearVelocity(DOUBLE2(0, m_ActPtr->GetLinearVelocity().y));
		m_ActPtr->SetPosition(DOUBLE2(GetWidth(), m_ActPtr->GetPosition().y));
	}

	if (m_ItemHoldingPtr != nullptr)
	{
		double offset = 12;
		if (m_ChangingDirectionsTimer.IsActive()) offset = 0;
		m_ItemHoldingPtr->SetPosition(m_ActPtr->GetPosition() + DOUBLE2(offset * m_DirFacing, 0));
		m_ItemHoldingPtr->SetLinearVelocity(DOUBLE2(0, 0));
	}

	m_WasOnGround = m_IsOnGround;
	m_DirFacingLastFrame = m_DirFacing;
}

// NOTE: Technically this isn't exactly the same as in the original, since when the player
// stands on the edge of a block they can't jump, but it's definitely close enough
bool Player::CalculateOnGround()
{
	if (m_ActPtr->GetLinearVelocity().y < 0) return false;

	DOUBLE2 point1 = m_ActPtr->GetPosition();
	DOUBLE2 point2 = m_ActPtr->GetPosition() + DOUBLE2(0, (GetHeight() / 2 + 3));
	DOUBLE2 intersection, normal;
	double fraction = -1.0;
	int collisionBits = Level::LEVEL | Level::BLOCK;

	if (m_LevelPtr->Raycast(point1, point2, collisionBits, intersection, normal, fraction))
	{
		return true;
	}

	return false;
}

void Player::TickAnimations(double deltaTime)
{
	m_AnimInfo.secondsElapsedThisFrame += deltaTime;
	if (m_AnimInfo.secondsElapsedThisFrame > m_AnimInfo.secondsPerFrame)
	{
		m_AnimInfo.secondsElapsedThisFrame -= m_AnimInfo.secondsPerFrame;
		m_AnimInfo.frameNumber++;

		switch (m_AnimationState)
		{
		case ANIMATION_STATE::WAITING:
		case ANIMATION_STATE::JUMPING:
		case ANIMATION_STATE::FALLING:
		case ANIMATION_STATE::CLIMBING:
		case ANIMATION_STATE::IN_PIPE:
		{
			if (m_IsRidingYoshi && m_RidingYoshiPtr->IsTongueStuckOut())
			{
				m_AnimInfo.frameNumber %= 4;
			}
			else
			{
				// NOTE: 1 frame animation
				m_AnimInfo.frameNumber = 0;
			}
		} break;
		case ANIMATION_STATE::WALKING:
		{
			if (m_IsRidingYoshi && m_RidingYoshiPtr->IsTongueStuckOut())
			{
				m_AnimInfo.frameNumber %= 4;
			}
			else if (m_ChangingDirectionsTimer.IsActive())
			{
				m_AnimInfo.frameNumber = 0;
			}
			else
			{
				m_AnimInfo.frameNumber %= 2;
			}
		} break;
		case ANIMATION_STATE::SPIN_JUMPING:
		{
			m_AnimInfo.frameNumber %= 4;
		} break;
		default:
		{
			OutputDebugString(String("ERROR: Unhandled animation state in Player::TickAnimations()\n"));
		} break;
		}
	}
}

void Player::Paint()
{
    MATRIX3X2 matPrevWorld = GAME_ENGINE->GetWorldMatrix();

	if (m_ExtraItemPtr != nullptr)
	{
		m_ExtraItemPtr->Paint();
	}

	if (m_InvincibilityTimer.IsActive() && m_InvincibilityTimer.FramesElapsed() % 10 < 5)
	{
		return; // NOTE: The player flashes while invincible
	}

	double centerX = m_ActPtr->GetPosition().x;
	double centerY = m_ActPtr->GetPosition().y;

	bool climbingFlip = m_AnimationState == ANIMATION_STATE::CLIMBING && m_LastClimbingPose == Direction::LEFT;

	bool changingDirections = m_ChangingDirectionsTimer.IsActive();
	bool firstHalf = m_ChangingDirectionsTimer.FramesElapsed() < m_ChangingDirectionsTimer.OriginalNumberOfFrames() / 2;
	bool yoshiFlip = m_RidingYoshiPtr &&
					 ((m_DirFacing == Direction::LEFT && !changingDirections) ||
					 (m_DirFacing == Direction::LEFT && changingDirections && !firstHalf) ||
					 (m_DirFacing == Direction::RIGHT && changingDirections && firstHalf));
	if (!changingDirections) firstHalf = false;

	if ((m_DirFacing == Direction::LEFT && !changingDirections) ||
		(m_DirFacing == Direction::RIGHT && changingDirections) ||
		climbingFlip || yoshiFlip)
	{
		MATRIX3X2 matReflect = MATRIX3X2::CreateScalingMatrix(DOUBLE2(-1, 1));
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(centerX, centerY);
		MATRIX3X2 matTranslateInverse = MATRIX3X2::CreateTranslationMatrix(-centerX, -centerY);
		GAME_ENGINE->SetWorldMatrix(matTranslateInverse * matReflect * matTranslate * matPrevWorld);
	}

	POWERUP_STATE powerupStateToPaint;
	double yo;
	if (m_PowerupTransitionTimer.Tick())
	{
		if (m_PowerupTransitionTimer.FramesElapsed() == 1)
		{
 			m_LevelPtr->SetPausedTimer(m_PowerupTransitionTimer.OriginalNumberOfFrames()-1);
		}

		yo = GetHeight() / 2 - (m_PrevPowerupState == POWERUP_STATE::NORMAL ? 6 : 4);
		
		if (m_PowerupTransitionTimer.FramesElapsed() % 12 > 6)
		{
			powerupStateToPaint = m_PrevPowerupState;
		}
		else
		{
			powerupStateToPaint = m_PowerupState;
		}
	}
	else
	{
		powerupStateToPaint = m_PowerupState;
		if (m_IsRidingYoshi)
		{
			yo = GetHeight() / 2 - 9.5;
		}
		else if (m_PowerupState == POWERUP_STATE::NORMAL)
		{
			yo = GetHeight() / 2 - 6;
		}
		else
		{ 
			yo = GetHeight() / 2 - 2;
		}
	}

	SpriteSheet* spriteSheetToPaint = GetSpriteSheetForPowerupState(powerupStateToPaint);
	INT2 spriteTile = CalculateAnimationFrame();
	spriteSheetToPaint->Paint(centerX, centerY - GetHeight() / 2 + yo, spriteTile.x, spriteTile.y);

	// Yoshi's Tongue
	if (m_IsRidingYoshi && m_RidingYoshiPtr->IsTongueStuckOut())
	{
		int toungeTimerFramesElapsed = m_RidingYoshiPtr->GetTongueTimer().FramesElapsed();
		int toungeTimerTotalFrames = m_RidingYoshiPtr->GetTongueTimer().OriginalNumberOfFrames();
		
		if (toungeTimerFramesElapsed > 12 &&
			toungeTimerFramesElapsed < toungeTimerTotalFrames - 7)
		{
			int tileWidth = SpriteSheetManager::yoshiWithMarioPtr->GetTileWidth();
			int tileHeight = SpriteSheetManager::yoshiWithMarioPtr->GetTileHeight();
			int srcX = 12 * tileWidth;
			int srcY = 0 * tileHeight;
			int width = int(m_RidingYoshiPtr->GetTongueLength() - 7);
			int height = tileHeight;
			int left = int(centerX - m_RidingYoshiPtr->GetWidth() / 2 + 31 - tileWidth / 2);
			int top = int(centerY - m_RidingYoshiPtr->GetHeight() / 2 - 1 - tileHeight / 2);
			RECT2 srcRect(srcX, srcY, srcX + width, srcY + height);
			// NOTE: We can't use the normal sprite sheet paint here since
			// we need to draw only part of yoshi's tounge at times
			GAME_ENGINE->DrawBitmap(SpriteSheetManager::yoshiWithMarioPtr->GetBitmap(), left, top, srcRect);

			// Tongue tip
			left += width - 3;
			top += tileHeight / 2 + 7;
			srcX = 422;
			srcY = 26;
			width = 7;
			height = 6;
			srcRect = RECT2(srcX, srcY, srcX + width, srcY + height);
			GAME_ENGINE->DrawBitmap(SpriteSheetManager::yoshiWithMarioPtr->GetBitmap(), left, top, srcRect);
		}
	}

	GAME_ENGINE->SetWorldMatrix(matPrevWorld);
}

INT2 Player::CalculateAnimationFrame()
{
	int srcX = 0, srcY = 0;

	if (m_IsDead)
	{
		srcX = 6;
		srcY = 1;
	}
	else if (m_IsHoldingItem)
	{
		if (m_IsDucking)
		{
			srcX = 1;
			srcY = 1;
		}
		else if (m_ChangingDirectionsTimer.IsActive())
		{
			srcX = 3;
			srcY = 2;
		}
		else if (m_IsLookingUp)
		{
			srcX = 0;
			srcY = 1;
		}
		else if (m_IsDucking)
		{
			srcX = 1;
			srcY = 1;
		}
		else
		{
			srcY = 1;

			if (m_AnimationState == ANIMATION_STATE::JUMPING) srcX = 3;
			else if (m_AnimationState == ANIMATION_STATE::WAITING) srcX = 2;
			else srcX = 2 + m_AnimInfo.frameNumber;
		}
	}
	else if (m_IsRidingYoshi)
	{
		bool playerIsSmall = m_PowerupState == POWERUP_STATE::NORMAL;
		srcY = playerIsSmall ? 0 : 1;

		if (m_RidingYoshiPtr->IsTongueStuckOut())
		{
			srcX = 8 + m_AnimInfo.frameNumber;
		}
		else if (m_ChangingDirectionsTimer.IsActive())
		{
			srcX = 4;
		}
		else
		{
			switch (m_AnimationState)
			{
			case ANIMATION_STATE::WAITING:
			{
				if (m_IsDucking)
				{
					srcX = 5;
				}
				else
				{
					srcX = 1;
				}
			} break;
			case ANIMATION_STATE::WALKING:
			{
				srcX = 1 + m_AnimInfo.frameNumber;
			} break;
			case ANIMATION_STATE::JUMPING:
			{
				srcX = 3;
			} break;
			case ANIMATION_STATE::FALLING:
			{
				srcX = 2;
			} break;
			}
		}
	}
	else
	{
		switch (m_AnimationState)
		{
		case ANIMATION_STATE::WAITING:
		{
			if (m_IsDead)
			{
				srcX = 6;
				srcY = 1;
			}
			else if (m_IsLookingUp)
			{
				srcX = 0;
				srcY = 0;
			}
			else if (m_IsDucking)
			{
				srcX = 1;
				srcY = 0;
			}
			else
			{
				srcX = 2;
				srcY = 0;
			}
		} break;
		case ANIMATION_STATE::WALKING:
		{
			if (m_ShellKickAnimationTimer.IsActive())
			{
				srcX = 5;
				srcY = 1;
			}
			else if (m_ChangingDirectionsTimer.IsActive())
			{
				srcX = 6;
				srcY = 0;
			}
			else if (m_IsRunning)
			{
				srcX = 4 + m_AnimInfo.frameNumber;
				srcY = 0;
			}
			else
			{
				srcX = 2 + m_AnimInfo.frameNumber;
				srcY = 0;
			}
		} break;
		case ANIMATION_STATE::JUMPING:
		{	
			if (m_IsDucking)
			{
				srcX = 1;
				srcY = 0;
			}
			else
			{
				srcX = 0;
				srcY = 2;
			}
		} break;
		case ANIMATION_STATE::SPIN_JUMPING:
		{
			srcX = 3 + m_AnimInfo.frameNumber;
			srcY = 2;
		} break;
		case ANIMATION_STATE::FALLING:
		{
			if (m_IsDucking)
			{
				srcX = 1;
				srcY = 0;
			}
			else if (m_IsRunning)
			{
				srcX = 2;
				srcY = 2;
			}
			else
			{
				srcX = 1;
				srcY = 2;
			}
		} break;
		case ANIMATION_STATE::CLIMBING:
		{
			if (m_PowerupState == POWERUP_STATE::NORMAL)
			{
				srcX = 7;
				srcY = 0;
			}
			else
			{
				srcX = 7;
				srcY = 2;
			}
		} break;
		case ANIMATION_STATE::IN_PIPE:
		{
			srcX = 3;
			srcY = 2;
		} break;
		}
	}

	return INT2(srcX, srcY);
}

bool Player::IsRidingYoshi()
{
	return m_IsRidingYoshi;
}

void Player::RideYoshi(Yoshi* yoshiPtr)
{
	assert(m_IsHoldingItem == false);
	assert(m_RidingYoshiPtr == nullptr);

	SoundManager::PlaySoundEffect(SoundManager::SOUND::YOSHI_PLAYER_MOUNT);

	m_RidingYoshiPtr = yoshiPtr;
	m_RidingYoshiPtr->SetCarryingPlayer(true, this);

	m_NeedsNewFixture = true;

	m_ChangingDirectionsTimer = CountdownTimer(YOSHI_TURN_AROUND_FRAMES);
	m_AnimInfo.secondsPerFrame = Yoshi::TONGUE_STUCK_OUT_SECONDS_PER_FRAME;

	m_IsRidingYoshi = true;
	m_AnimationState = ANIMATION_STATE::WAITING;
}

void Player::DismountYoshi(bool runWild)
{
	assert(m_RidingYoshiPtr != nullptr);

	if (runWild)
	{
		m_ActPtr->SetLinearVelocity(DOUBLE2(0, -350));
		m_RidingYoshiPtr->RunWild();

		SoundManager::PlaySoundEffect(SoundManager::SOUND::YOSHI_RUN_AWAY);
	}
	else
	{
		m_ActPtr->SetPosition(m_ActPtr->GetPosition() + DOUBLE2(0, -4));
		m_RidingYoshiPtr->SetCarryingPlayer(false);
	}
	m_RidingYoshiPtr = nullptr;

	m_NeedsNewFixture = true;

	m_ChangingDirectionsTimer = CountdownTimer(3);
	m_AnimInfo.secondsPerFrame = MARIO_SECONDS_PER_FRAME;

	m_IsRidingYoshi = false;
}

void Player::OnItemPickup(Item* itemPtr, Level* levelPtr)
{
	switch (itemPtr->GetType())
	{
	case Item::TYPE::COIN:
	{
		AddCoin(levelPtr);
		Coin* coinPtr = (Coin*)itemPtr;
		if (coinPtr->GetLifeRemaining() == -1)
		{
			coinPtr->GenerateParticles();
		}

		AddScore(10, coinPtr->GetPosition() + DOUBLE2(5, -12));
	} break;
	case Item::TYPE::DRAGON_COIN:
	{
		AddDragonCoin(levelPtr);
		DragonCoin* dragonCoinPtr = (DragonCoin*)itemPtr;
		dragonCoinPtr->GenerateParticles();
		AddScore(2000, dragonCoinPtr->GetPosition() + DOUBLE2(5, -12)); // NOTE: Not quite perfectly accurate, but enough for now
	} break;
	case Item::TYPE::SUPER_MUSHROOM:
	{
		AddScore(1000, m_ActPtr->GetPosition());

		switch (m_PowerupState)
		{
		case POWERUP_STATE::NORMAL:
		{
			SoundManager::PlaySoundEffect(SoundManager::SOUND::PLAYER_SUPER_MUSHROOM_COLLECT);
			m_SpriteSheetPtr = SpriteSheetManager::superMarioPtr;
			ChangePowerupState(POWERUP_STATE::SUPER);
		} break;
		case POWERUP_STATE::SUPER:
		case POWERUP_STATE::FIRE:
		case POWERUP_STATE::BALLOON:
		case POWERUP_STATE::CAPE:
		case POWERUP_STATE::STAR:
		{
			m_ExtraItemToBeSpawnedType = itemPtr->GetType();
		} break;
		}
	} break;
	case Item::TYPE::CAPE_FEATHER:
	{
		ChangePowerupState(POWERUP_STATE::CAPE);
	} break;
	case Item::TYPE::FIRE_FLOWER:
	{
		ChangePowerupState(POWERUP_STATE::FIRE);
	} break;
	case Item::TYPE::POWER_BALLOON:
	{
		ChangePowerupState(POWERUP_STATE::BALLOON);
	} break;
	case Item::TYPE::ONE_UP_MUSHROOM:
	{
		m_Lives++;
	} break;
	case Item::TYPE::THREE_UP_MOON:
	{
		m_Lives += 3;
	} break;
	default:
	{
		OutputDebugString(String("ERROR: Unhandled item passed to Player::OnItemPickup\n"));
	} break;
	}
}

void Player::MidwayGatePasshrough()
{
	if (m_PowerupState == POWERUP_STATE::NORMAL)
	{
		m_PowerupState = POWERUP_STATE::SUPER;

		m_NeedsNewFixture = true;
	}
}

void Player::ChangePowerupState(POWERUP_STATE newPowerupState, bool isUpgrade)
{
	m_PrevPowerupState = m_PowerupState;

	m_PowerupState = newPowerupState;
	m_SpriteSheetPtr = GetSpriteSheetForPowerupState(m_PowerupState);

	m_PowerupTransitionTimer.Start();

	if (isUpgrade)
	{
		// TODO: Play sound effect here
		// NOTE: Sound effect changes based on what state you were in before!
	}
	else
	{
		// TODO: Play sound effect here
		// NOTE: Sound effect changes based on what state you were in before!
	}
	
	m_NeedsNewFixture = true;
}

SpriteSheet* Player::GetSpriteSheetForPowerupState(POWERUP_STATE powerupState)
{
	if (m_IsRidingYoshi)
	{
		return SpriteSheetManager::yoshiWithMarioPtr;
	}
	else
	{
		switch (powerupState)
		{
		case POWERUP_STATE::NORMAL:
			return SpriteSheetManager::smallMarioPtr;
		case POWERUP_STATE::SUPER:
			return SpriteSheetManager::superMarioPtr;
		default:
			OutputDebugString(String("Unhandled powerup state passed to Player::GetSpriteSheetForPowerupState!\n"));
			return nullptr;
		}
	}
}

void Player::AddItemToBeHeld(Item* itemPtr)
{
	assert(m_ItemHoldingPtr == nullptr);

	m_ItemHoldingPtr = itemPtr;
	m_IsHoldingItem = true;
}

void Player::KickShell(KoopaShell* koopaShellPtr, bool wasThrown)
{
	int direction;
	if (wasThrown) 
	{
		direction = m_DirFacing;
	}
	else 
	{
		direction = (m_ActPtr->GetPosition().x > koopaShellPtr->GetPosition().x) ? Direction::LEFT : Direction::RIGHT;

		m_LevelPtr->GetPlayer()->AddScore(200, koopaShellPtr->GetPosition() + DOUBLE2(0, -12));
	}
	koopaShellPtr->KickHorizontally(direction, wasThrown);
}

void Player::DropHeldItem()
{
	assert(m_IsHoldingItem);

	switch (m_ItemHoldingPtr->GetType())
	{
	case Item::TYPE::KOOPA_SHELL:
	{
		((KoopaShell*)m_ItemHoldingPtr)->ShellHit();
	} break;
	case Item::TYPE::GRAB_BLOCK:
	{
		((GrabBlock*)m_ItemHoldingPtr)->SetMoving(DOUBLE2(100, 0));
	} break;
	}
	m_ItemHoldingPtr = nullptr;
	m_IsHoldingItem = false;
}

void Player::KickHeldItem(bool gently)
{
	assert(m_ItemHoldingPtr != nullptr);

	if (gently)
	{
		((KoopaShell*)m_ItemHoldingPtr)->SetLinearVelocity(m_ActPtr->GetLinearVelocity() + DOUBLE2(m_DirFacing * 30, 0));
	}
	else
	{
		if (GAME_ENGINE->IsKeyboardKeyDown(VK_UP))
		{
			double horizontalVel = RUN_BASE_VEL * m_DirFacing;
			if (m_ActPtr->GetLinearVelocity().x == 0.0) horizontalVel = 0.0;
			((KoopaShell*)m_ItemHoldingPtr)->KickVertically(horizontalVel);
		}
		else
		{
			((KoopaShell*)m_ItemHoldingPtr)->KickHorizontally(m_DirFacing, true);
		}

		m_ShellKickAnimationTimer.Start();
	}

	m_ItemHoldingPtr = nullptr;

	m_IsHoldingItem = false;
	m_ShellKickAnimationTimer.Start();
}

void Player::AddCoin(Level* levelPtr, bool playSound)
{
	m_Coins++;

	if (playSound)
	{
		SoundManager::PlaySoundEffect(SoundManager::SOUND::COIN_COLLECT);
	}
	
	if (m_Coins > 99)
	{
		m_Coins = 0;
		AddLife();
		// LATER: Play sound here
	}
}

void Player::AddDragonCoin(Level* levelPtr)
{
	AddCoin(levelPtr, false);
	m_DragonCoins++;

	SoundManager::PlaySoundEffect(SoundManager::SOUND::DRAGON_COIN_COLLECT);

	if (m_DragonCoins >= 5)
	{
		m_DragonCoins = 0;
		AddLife();
		levelPtr->SetAllDragonCoinsCollected(true);
	}
}

void Player::AddLife()
{
	m_Lives++;
	// LATER: Play sound here
}

void Player::Bounce()
{
	m_ActPtr->SetLinearVelocity(DOUBLE2(m_ActPtr->GetLinearVelocity().x, BOUNCE_VEL));
}

void Player::Die()
{
	// TODO: Lives don't decrement right as the player dies, but after the screen
	// goes dark and right before the map is shown
	m_Lives--;
	if (m_Lives < 0)
	{
		// LATER: Trigger game over screen
		m_Lives = STARTING_LIVES;
	}

	m_DeathAnimationTimer.Start();
	m_AnimationState = ANIMATION_STATE::WAITING;
	m_IsDead = true;

	m_ActPtr->SetSensor(true);
	
	SoundManager::SetAllSongsPaused(true);
	SoundManager::PlaySoundEffect(SoundManager::SOUND::PLAYER_DEATH);
}

void Player::TakeDamage()
{
	if (m_IsInvincible) return;

	if (m_IsRidingYoshi) 
	{
		DismountYoshi(true);

		return;
	}

	switch (m_PowerupState)
	{
	case POWERUP_STATE::NORMAL:
	{
		Die();
	} break;
	case POWERUP_STATE::SUPER:
	case POWERUP_STATE::CAPE:
	case POWERUP_STATE::FIRE:
	case POWERUP_STATE::BALLOON:
	{
		SoundManager::PlaySoundEffect(SoundManager::SOUND::PLAYER_DAMAGE);
		ChangePowerupState(POWERUP_STATE::NORMAL, false);
		m_InvincibilityTimer.Start();
		m_IsInvincible = true;
	} break;
	case POWERUP_STATE::STAR:
	{
		// NOTE: We're invincible! :D
		// TODO: Remove item we hit
	} break;
	default:
	{
		OutputDebugString(String("ERROR: Unhandled powerup state in Player::TakeDamage()\n"));
	} break;
	}
}

void Player::AddScore(int score, DOUBLE2 particlePosition)
{
	if (score < 0)
	{
		OutputDebugString(String("ERROR: Negative score attempted to be added to player! ") + String(score) + String("\n"));
		return;
	}

	m_Score += score;

	NumberParticle* numberParticlePtr = new NumberParticle(score, particlePosition);
	m_LevelPtr->AddParticle(numberParticlePtr);
}

void Player::SetTouchingGrabBlock(bool touching, GrabBlock* grabBlockPtr)
{
	if (touching)
	{
		if (m_RecentlyTouchedGrabBlocksPtrArr[0] != nullptr)
		{
			if (m_RecentlyTouchedGrabBlocksPtrArr[1] == nullptr)
			{
				m_RecentlyTouchedGrabBlocksPtrArr[1] = m_RecentlyTouchedGrabBlocksPtrArr[0];
				m_RecentlyTouchedGrabBlocksPtrArr[0] = grabBlockPtr;
			}
		}
		else
		{
			m_RecentlyTouchedGrabBlocksPtrArr[0] = grabBlockPtr;
		}
	}
	else
	{
		if (m_RecentlyTouchedGrabBlocksPtrArr[0] == grabBlockPtr)
		{
			m_RecentlyTouchedGrabBlocksPtrArr[0] = nullptr;
			if (m_RecentlyTouchedGrabBlocksPtrArr[1] != nullptr)
			{
				m_RecentlyTouchedGrabBlocksPtrArr[0] = m_RecentlyTouchedGrabBlocksPtrArr[1];
				m_RecentlyTouchedGrabBlocksPtrArr[1] = nullptr;
			}
		}
		else if (m_RecentlyTouchedGrabBlocksPtrArr[1] == grabBlockPtr)
		{
			m_RecentlyTouchedGrabBlocksPtrArr[1] = nullptr;
		}
		else
		{
			OutputDebugString(String("ERROR: Player::SetTouchingGrabBlock called with unknown grab block!\n"));
		}
	}
}

void Player::SetOverlappingWithBeanstalk(bool overlapping)
{
	m_IsOverlappingWithBeanstalk = overlapping;
	
	if (overlapping == false && m_AnimationState == ANIMATION_STATE::CLIMBING)
	{
		SetClimbingBeanstalk(false);
	}
}

void Player::SetClimbingBeanstalk(bool climbing)
{
	if (climbing)
	{
		m_ActPtr->SetGravityScale(0.0);
		m_ActPtr->SetLinearVelocity(DOUBLE2(0, 0));
		m_AnimationState = ANIMATION_STATE::CLIMBING;
	}
	else
	{
		m_ActPtr->SetGravityScale(9.8);
		if (m_IsOnGround) m_AnimationState = ANIMATION_STATE::WALKING;
		else m_AnimationState = ANIMATION_STATE::FALLING;
	}

	m_FramesClimbingSinceLastFlip = FRAMES_OF_CLIMBING_ANIMATION;
}

int Player::GetWidth()
{
	static const int SMALL_WIDTH = 9;
	static const int LARGE_WIDTH = 9;

	if (m_IsRidingYoshi)
	{
		return m_RidingYoshiPtr->GetWidth();
	}
	else if (m_PowerupState == POWERUP_STATE::NORMAL)
	{
		return SMALL_WIDTH;
	}
	else
	{
		return LARGE_WIDTH;
	}
}

int Player::GetHeight()
{
	static const int SMALL_HEIGHT = 15;
	static const int LARGE_HEIGHT = 23;

	if (m_IsRidingYoshi)
	{
		return m_RidingYoshiPtr->GetHeight();
	}
	else if (m_PowerupState == POWERUP_STATE::NORMAL)
	{
		return SMALL_HEIGHT;
	}
	else
	{
		return LARGE_HEIGHT;
	}
}


bool Player::IsInvincible()
{
	return m_IsInvincible;
}

Item* Player::GetHeldItemPtr()
{
	return m_ItemHoldingPtr;
}

bool Player::IsHoldingItem()
{
	return m_IsHoldingItem;
}

Item::TYPE Player::GetExtraItemType()
{
	if (m_ExtraItemPtr != nullptr)
	{
		return m_ExtraItemPtr->GetType();
	}
	else
	{
		return Item::TYPE::NONE;
	}
}

void Player::ReleaseExtraItem(DOUBLE2 position)
{
	m_ExtraItemToBeSpawnedType = Item::TYPE::NONE;

	delete m_ExtraItemPtr;

	SuperMushroom* extraSuperMushroomPtr = new SuperMushroom(position, m_LevelPtr, Direction::RIGHT);

	// TODO: FIXME: Mushroom needs custom physics here to slowly fall down through the screen
	
	m_ExtraItemPtr = extraSuperMushroomPtr;
}

bool Player::AttemptToEnterPipes()
{
	if (m_TouchingPipe)
	{
		if (GAME_ENGINE->IsKeyboardKeyDown(VK_DOWN) &&
			m_PipeTouchingPtr->GetOrientation() == Pipe::Orientation::UP &&
			m_PipeTouchingPtr->IsPlayerInPositionToEnter(this))
		{
			EnterPipe();
			return true;
		}
		else if (GAME_ENGINE->IsKeyboardKeyDown(VK_UP) &&
			m_PipeTouchingPtr->GetOrientation() == Pipe::Orientation::DOWN &&
			m_PipeTouchingPtr->IsPlayerInPositionToEnter(this))
		{
			EnterPipe();
			return true;
		}
		else if (GAME_ENGINE->IsKeyboardKeyDown(VK_RIGHT) &&
			m_PipeTouchingPtr->GetOrientation() == Pipe::Orientation::LEFT &&
			m_PipeTouchingPtr->IsPlayerInPositionToEnter(this))
		{
			EnterPipe();
			return true;
		}
		else if (GAME_ENGINE->IsKeyboardKeyDown(VK_LEFT) &&
			m_PipeTouchingPtr->GetOrientation() == Pipe::Orientation::RIGHT &&
			m_PipeTouchingPtr->IsPlayerInPositionToEnter(this))
		{
			EnterPipe();
			return true;
		}
	}
	return false;
}

void Player::EnterPipe()
{
	SoundManager::PlaySoundEffect(SoundManager::SOUND::PLAYER_DAMAGE); // Same sound as taking damage

	m_AnimationState = ANIMATION_STATE::IN_PIPE;
	m_ActPtr->SetSensor(true);
	m_ActPtr->SetGravityScale(0.0);
	m_LevelPtr->SetPaused(true);
	m_EnteringPipeTimer.Start();
}

void Player::SetTouchingPipe(bool touching, Pipe* pipePtr)
{
	m_TouchingPipe = touching;

	if (touching)
	{
		m_PipeTouchingPtr = pipePtr;
	}
	else 
	{
		// When we start to enter a pipe, end contact is called since we turn into a sensor,
		// but we don't want to forget which pipe we're entering
		if (m_AnimationState != ANIMATION_STATE::IN_PIPE)
		{
			m_PipeTouchingPtr = nullptr;
		}
	}
}

void Player::SetExitingPipe(Pipe* pipePtr)
{
	m_PipeTouchingPtr = pipePtr;
	m_ActPtr->SetSensor(true);
	m_ActPtr->SetGravityScale(0.0);
	m_ExitingPipeTimer.Start();
	m_AnimationState = ANIMATION_STATE::IN_PIPE;
}

void Player::SetPosition(DOUBLE2 newPosition)
{
	m_ActPtr->SetPosition(newPosition);
}

Player::ANIMATION_STATE Player::GetAnimationState()
{
	return m_AnimationState;
}

void Player::ResetNumberOfFramesUntilEndStompSound()
{
	m_HeadStompSoundDelayTimer.Start();
}

bool Player::IsOnGround()
{
	return m_IsOnGround;
}

DOUBLE2 Player::GetPosition()
{
	return m_ActPtr->GetPosition();
}

int Player::GetDirectionFacing()
{
	return m_DirFacing;
}

DOUBLE2 Player::GetLinearVelocity()
{
	return m_ActPtr->GetLinearVelocity();
}

void Player::SetLinearVelocity(const DOUBLE2& newLinearVelRef)
{
	m_ActPtr->SetLinearVelocity(newLinearVelRef);
}

int Player::GetLives()
{
	return m_Lives;
}

int Player::GetCoinsCollected()
{
	return m_Coins;
}

int Player::GetDragonCoinsCollected()
{
	return m_DragonCoins;
}

int Player::GetStarsCollected()
{
	return m_Stars;
}

int Player::GetScore()
{
	return m_Score;
}

bool Player::IsDead()
{
	return m_IsDead;
}

bool Player::IsRunning()
{
	return m_IsRunning;
}

Player::POWERUP_STATE Player::GetPowerupState()
{
	return m_PowerupState;
}

Player::POWERUP_STATE Player::StringToPowerupState(std::string powerupStateStr)
{
	if (!powerupStateStr.compare("Normal")) return POWERUP_STATE::NORMAL;
	else if (!powerupStateStr.compare("Super")) return POWERUP_STATE::SUPER;
	else if (!powerupStateStr.compare("Fire")) return POWERUP_STATE::FIRE;
	else if (!powerupStateStr.compare("Cape")) return POWERUP_STATE::CAPE;
	else if (!powerupStateStr.compare("Balloon")) return POWERUP_STATE::BALLOON;
	else if (!powerupStateStr.compare("Star")) return POWERUP_STATE::STAR;
	else return POWERUP_STATE::NONE;
}

std::string Player::PowerupStateToString(POWERUP_STATE powerupState)
{
	switch (powerupState)
	{
	case POWERUP_STATE::NORMAL: return "Normal";
	case POWERUP_STATE::SUPER: return "Super";
	case POWERUP_STATE::FIRE: return "Fire";
	case POWERUP_STATE::CAPE: return "Cape";
	case POWERUP_STATE::BALLOON: return "Balloon";
	case POWERUP_STATE::STAR: return "Star";
	default: return "INVALID STATE";
	}
}

String Player::AnimationStateToString(ANIMATION_STATE state)
{
	String str;

	switch (state)
	{
	case ANIMATION_STATE::WAITING:
		str = String("waiting");
		break;
	case ANIMATION_STATE::WALKING:
		str = String("walking");
		break;
	case ANIMATION_STATE::JUMPING:
		str = String("jumping");
		break;
	case ANIMATION_STATE::SPIN_JUMPING:
		str = String("spin_jumping");
		break;
	case ANIMATION_STATE::FALLING:
		str = String("falling");
		break;
	case ANIMATION_STATE::CLIMBING:
		str = String("climbing");
		break;
	case ANIMATION_STATE::NONE:
	default:
		return String("unknown state passed to Player::AnimationStateToString: ") + String(int(state));
	}

	if (m_IsRunning) str += String(" running");
	if (m_ChangingDirectionsTimer.IsActive()) str += String(" chng_dirs");
	if (m_IsDead) str += String(" dying");
	if (m_IsDucking) str += String(" ducking");
	if (m_IsLookingUp) str += String(" lkng_up");
	if (m_IsHoldingItem) str += String(" hld_item");

	return str;
}

bool Player::IsAirborne()
{
	return	m_AnimationState == ANIMATION_STATE::JUMPING ||
			m_AnimationState == ANIMATION_STATE::SPIN_JUMPING ||
			m_AnimationState == ANIMATION_STATE::FALLING;
}


bool Player::IsTransitioningPowerups()
{
	return m_PowerupTransitionTimer.IsActive();
}

bool Player::IsDucking()
{
	return m_IsDucking;
}
