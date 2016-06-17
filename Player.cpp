#include "stdafx.h"

#include "Player.h"
#include "Game.h"
#include "GameState.h"
#include "StateManager.h"
#include "Enumerations.h"
#include "SpriteSheetManager.h"
#include "GameSession.h"
#include "Keybindings.h"
#include "SpriteSheet.h"

#include "SoundManager.h"
#include "DustParticle.h"
#include "NumberParticle.h"
#include "SplatParticle.h"
#include "OneUpParticle.h"
#include "GrabBlock.h"
#include "PSwitch.h"

#include "SuperMushroom.h"
#include "Coin.h"
#include "DragonCoin.h"
#include "KoopaShell.h"
#include "Yoshi.h"
#include "Pipe.h"

// Static Initializations
const double Player::FRICTION = 0.2;
const int Player::WALKING_ACCELERATION = 40000;
const int Player::RUNNING_ACCELERATION = 30000;
const int Player::MAX_WALK_VEL = 120;
const int Player::MAX_RUN_VEL = 240;
const int Player::JUMP_VEL = -15000;
const int Player::BEANSTALK_JUMP_VEL = -21500;
const int Player::MAX_FALL_VEL = 220;
const int Player::BOUNCE_VEL = -195;
const int Player::BOUNCE_VEL_HORIZONTAL = 50;
const int Player::STARTING_LIVES = 5;
const int Player::YOSHI_DISMOUNT_XVEL = 2500;
const int Player::YOSHI_TURN_AROUND_FRAMES = 15;
const int Player::DEATH_Y_VEL = -420;

const double Player::MARIO_SECONDS_PER_FRAME = 0.065;
const double Player::MARIO_SPINNING_SECONDS_PER_FRAME = 0.04;

Player::Player(Level* levelPtr, GameState* gameStatePtr, SessionInfo sessionInfo) :
	Entity(DOUBLE2(), BodyType::DYNAMIC, levelPtr, ActorId::PLAYER, this),
	m_GameStatePtr(gameStatePtr)
{
	Reset();

	m_AnimInfo.secondsPerFrame = MARIO_SECONDS_PER_FRAME;

	m_RecentlyTouchedGrabBlocksPtrArr = std::vector<GrabBlock*>(2);

	if (sessionInfo.m_PlayerLives == -1) // The session info was not set, use default values
	{
		m_Lives = STARTING_LIVES;
	}
	else // The session info was set, use it
	{
		m_Lives = sessionInfo.m_PlayerLives;
		m_Score = sessionInfo.m_PlayerScore;
		m_Coins = sessionInfo.m_CoinsCollected;
		m_DragonCoins = sessionInfo.m_DragonCoinsCollected;
		m_RedStars = sessionInfo.m_RedStarsCollected;
		m_IsRidingYoshi = (sessionInfo.m_PlayerRidingYoshi == 1);
		if (m_IsRidingYoshi) m_RidingYoshiPtr = m_LevelPtr->GetYoshiPtr();
		m_IsHoldingItem = sessionInfo.m_HeldItemType.length() > 0;
		if (m_IsHoldingItem) m_ItemHoldingPtr = Item::StringToItem(sessionInfo.m_HeldItemType, m_ActPtr->GetPosition(), m_LevelPtr);
		bool differentPowerupState = sessionInfo.m_PlayerPowerupState.length() > 0;
		if (differentPowerupState) m_PowerupState = StringToPowerupState(sessionInfo.m_PlayerPowerupState);
		else m_PowerupState = PowerupState::NORMAL;
	}

	b2Filter collisionFilter;
	collisionFilter.categoryBits = Level::PLAYER;
	collisionFilter.maskBits = Level::LEVEL | Level::BLOCK | Level::YOSHI | Level::BEANSTALK | Level::ENEMY | Level::ITEM | Level::SHELL;
	m_ActPtr->SetCollisionFilter(collisionFilter);
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
	m_RedStars = 0;
	m_DragonCoins = 0;

	m_NeedsNewFixture = true;

	m_ExtraItemPtr = nullptr;
	m_ExtraItemToBeSpawnedType = Item::Type::NONE;

	m_ItemHoldingPtr = nullptr;

	m_FramesSpentInAir = -1;
	m_LastScoreAdded = -1;

	m_DeathAnimationTimer = SMWTimer(240);
	m_ItemKickAnimationTimer = SMWTimer(10);
	m_HeadStompSoundDelayTimer = SMWTimer(10);
	m_PowerupTransitionTimer = SMWTimer(50);
	m_InvincibilityTimer = SMWTimer(120);
	m_SpawnDustCloudTimer = SMWTimer(4);
	m_ChangingDirectionsTimer = SMWTimer(3);
	m_EnteringPipeTimer = SMWTimer(60);
	m_ExitingPipeTimer = SMWTimer(35);
	m_ScoreAddedTimer = SMWTimer(15);

	m_AnimationState = AnimationState::WAITING;
	m_IsDucking = false;
	m_IsLookingUp = false;
	m_IsRunning = false;
	m_IsHoldingItem = false;
	m_IsDead = false;
	m_IsRidingYoshi = false;
	m_IsOverlappingWithBeanstalk = false;
	m_IsInvincible = false;

	m_PowerupState = PowerupState::NORMAL;
	m_PrevPowerupState = PowerupState::NORMAL;
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
	if (Game::DEBUG_TELEPORT_PLAYER_TO_POSITION != -1)
	{
		if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::DEBUG_TELEPORT_PLAYER))
		{
			m_ActPtr->SetPosition(DOUBLE2(Game::DEBUG_TELEPORT_PLAYER_TO_POSITION, 250));
		}
	}

	if (m_IsDead)
	{
		if (m_DeathAnimationTimer.Tick() && m_DeathAnimationTimer.IsComplete())
		{
			m_LevelPtr->Reset();
			return;
		}

		if (m_DeathAnimationTimer.FramesElapsed() == 1)
		{
			m_LevelPtr->SetPaused(true, true);
		}

		// NOTE: The player doesn't fly upward until after a short delay
		if (m_DeathAnimationTimer.FramesElapsed() == 40)
		{
			m_ActPtr->SetActive(true);
			m_ActPtr->SetLinearVelocity(DOUBLE2(0, DEATH_Y_VEL));
		}
	}

	if (m_EnteringPipeTimer.Tick())
	{
		// Move delta units per frame while in a pipe
		const double delta = 0.8;
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
			
			m_GameStatePtr->EnterNewLevel(m_PipeTouchingPtr, currentSessionInfo);
		}
		return;
	}
	if (m_ExitingPipeTimer.Tick())
	{
		const double delta = 0.6;
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
			m_ActPtr->SetGravityScale(1.0);
			m_LevelPtr->SetPaused(false, false);
			m_AnimationState = AnimationState::WAITING;
		}
		return;
	}

	if (m_HeadStompSoundDelayTimer.Tick() && m_HeadStompSoundDelayTimer.IsComplete())
	{
		SoundManager::PlaySoundEffect(SoundManager::Sound::ENEMY_HEAD_STOMP_END);
	}

	m_PowerupTransitionTimer.Tick();
	m_ChangingDirectionsTimer.Tick();
	m_ItemKickAnimationTimer.Tick();
	m_SpawnDustCloudTimer.Tick();
	
	if (m_ScoreAddedTimer.Tick() && m_ScoreAddedTimer.IsComplete())
	{
		m_LastScoreAdded = -1;
	}

	if (m_InvincibilityTimer.Tick() && m_InvincibilityTimer.IsComplete())
	{
		m_IsInvincible = false;
	}

	if (m_NeedsNewFixture)
	{
		const double oldHalfHeight = double(GetHeight()) / 2.0;

		b2Fixture* fixturePtr = m_ActPtr->GetBody()->GetFixtureList();
		while (fixturePtr != nullptr)
		{
			b2Fixture* nextFixturePtr = fixturePtr->GetNext();
			m_ActPtr->GetBody()->DestroyFixture(fixturePtr);
			fixturePtr = nextFixturePtr;
		}

		m_ActPtr->SetActive(true);
		m_ActPtr->AddBoxFixture(GetWidth(), GetHeight(), 0.0, FRICTION);

		const double newHalfHeight = double(GetHeight()) / 2.0;
		const double newCenterY = m_ActPtr->GetPosition().y + (oldHalfHeight - newHalfHeight) - 1;
		m_ActPtr->SetPosition(DOUBLE2(m_ActPtr->GetPosition().x, newCenterY));

		m_NeedsNewFixture = false;
	}

	HandleKeyboardInput(deltaTime);

	if (m_AnimationState != AnimationState::CLIMBING &&
		m_IsOverlappingWithBeanstalk &&
		(GAME_ENGINE->IsKeyboardKeyDown(Keybindings::D_PAD_UP) ||
		(!m_IsOnGround && GAME_ENGINE->IsKeyboardKeyDown(Keybindings::D_PAD_DOWN))))
	{
		SetClimbingBeanstalk(true);
	}

	if (m_IsHoldingItem)
	{
		// NOTE: Once the player is no longer running, they should kick the shell
		if (m_IsRunning == false)
		{
			// When ducking, the player gently releases the item
			KickHeldItem(deltaTime, m_IsDucking);
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
	if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::B_BUTTON)) // Regular jump
	{
		SetClimbingBeanstalk(false);
		m_AnimationState = AnimationState::JUMPING;
		SoundManager::PlaySoundEffect(SoundManager::Sound::PLAYER_JUMP);
		m_FramesSpentInAir = 0;
		m_ActPtr->SetLinearVelocity(DOUBLE2(0, BEANSTALK_JUMP_VEL * deltaTime));
		return;
	}

	DOUBLE2 prevPlayerPos = m_ActPtr->GetPosition();
	bool moved = true;
	if (GAME_ENGINE->IsKeyboardKeyDown(Keybindings::D_PAD_UP))
	{
		m_ActPtr->SetPosition(prevPlayerPos.x, prevPlayerPos.y - 1);
	}
	else if (GAME_ENGINE->IsKeyboardKeyDown(Keybindings::D_PAD_DOWN))
	{
		m_ActPtr->SetPosition(prevPlayerPos.x, prevPlayerPos.y + 1);

		m_IsOnGround = CalculateOnGround();
		if (m_IsOnGround)
		{
			SetClimbingBeanstalk(false);
			return;
		}
	} 
	else if (GAME_ENGINE->IsKeyboardKeyDown(Keybindings::D_PAD_LEFT))
	{
		m_ActPtr->SetPosition(prevPlayerPos.x - 1, prevPlayerPos.y);
	}
	else if (GAME_ENGINE->IsKeyboardKeyDown(Keybindings::D_PAD_RIGHT))
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

void Player::HandleKeyboardInput(double deltaTime)
{
	if (m_IsDead) return; // NOTE: The player can't do much now...
	
	if (m_AnimationState == AnimationState::CLIMBING)
	{
		HandleClimbingStateKeyboardInput(deltaTime);
		return;
	}

	if (AttemptToEnterPipes()) return;

	int maxXVel = (m_IsRunning ? MAX_RUN_VEL : MAX_WALK_VEL);
	// How fast we would be going if we had instant acceleration
	double targetXVel = 0.0,
		   targetYVel = 0.0;

	if (m_IsDucking &&
		GAME_ENGINE->IsKeyboardKeyDown(Keybindings::D_PAD_DOWN) == false)
	{
		m_AnimationState = AnimationState::WAITING;
		m_IsDucking = false;
	}
	if (m_IsLookingUp &&
		GAME_ENGINE->IsKeyboardKeyDown(Keybindings::D_PAD_UP) == false)
	{
		m_AnimationState = AnimationState::WAITING;
		m_IsLookingUp = false;
	}

	const bool regularJumpKeyPressed = GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::B_BUTTON);
	const bool spinJumpKeyPressed = GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::A_BUTTON);

	if (m_IsRidingYoshi)
	{
		if (spinJumpKeyPressed)
		{
			targetYVel = JUMP_VEL * deltaTime;
			if (m_RidingYoshiPtr->IsAirborne())
			{
				m_AnimationState = AnimationState::JUMPING;
				DismountYoshi();
			}
			else
			{
				m_AnimationState = AnimationState::SPIN_JUMPING;
				m_AnimInfo.secondsPerFrame = MARIO_SPINNING_SECONDS_PER_FRAME;
				m_DirFacing = -m_RidingYoshiPtr->GetDirectionFacing();
				targetXVel = m_DirFacing * YOSHI_DISMOUNT_XVEL;
				SoundManager::PlaySoundEffect(SoundManager::Sound::PLAYER_SPIN_JUMP);
				DismountYoshi(false, -m_RidingYoshiPtr->GetDirectionFacing() * 80);
			}
			m_FramesSpentInAir = 0;
		}
	}
	
	m_IsOnGround = CalculateOnGround();
	if (m_IsOnGround)
	{
		if (m_WasOnGround == false)
		{
			m_AnimationState = AnimationState::WAITING;
		}

		if (m_AnimationState != AnimationState::SPIN_JUMPING &&
			regularJumpKeyPressed ||
			(spinJumpKeyPressed && m_IsHoldingItem)) 
		{
			// Regular jump
			m_AnimationState = AnimationState::JUMPING;
			m_IsOnGround = false;
			SoundManager::PlaySoundEffect(SoundManager::Sound::PLAYER_JUMP);
			m_FramesSpentInAir = 0;
			// The player jumps higher the faster they are running
			const double jumpVel = JUMP_VEL * deltaTime + (abs(m_ActPtr->GetLinearVelocity().x) / double(MAX_RUN_VEL)) * -25.0; 
			targetYVel = jumpVel;
			m_IsLookingUp = false;
		}
		else if (spinJumpKeyPressed && m_IsRidingYoshi == false) 
		{
			// Spin jump
			m_AnimationState = AnimationState::SPIN_JUMPING;
			m_AnimInfo.secondsPerFrame = MARIO_SPINNING_SECONDS_PER_FRAME;
			m_IsOnGround = false;
			SoundManager::PlaySoundEffect(SoundManager::Sound::PLAYER_SPIN_JUMP);
			m_FramesSpentInAir = 0;
			targetYVel = JUMP_VEL * deltaTime;
			m_IsLookingUp = false;
		}
		else
		{
			m_FramesSpentInAir = -1;
		}
	}
	else // Player is not on the ground
	{
		m_FramesSpentInAir++;

		if (m_ActPtr->GetLinearVelocity().y < -100.0)
		{
			// The player is still rising and can hold down the jump key to jump higher
			if (GAME_ENGINE->IsKeyboardKeyDown(Keybindings::B_BUTTON) ||
				GAME_ENGINE->IsKeyboardKeyDown(Keybindings::A_BUTTON))
			{
				// NOTE: gravityScale is close to 1 at the start of the jump
				// and goes towards 0 near the apex
				double gravityScale = (m_FramesSpentInAir / 12.0) * 0.5;

				// Saturate [0.0, 1.0]
				gravityScale = max(0.0, min(1.0, gravityScale));

				m_ActPtr->SetGravityScale(gravityScale);
			}
			else
			{
				m_ActPtr->SetGravityScale(1.0);
			}
		}
		else
		{
			m_ActPtr->SetGravityScale(1.0);
			if (m_AnimationState != AnimationState::SPIN_JUMPING)
			{
				m_AnimationState = AnimationState::FALLING;
				m_AnimInfo.secondsPerFrame = MARIO_SECONDS_PER_FRAME;
			}
		}
	}

	if (GAME_ENGINE->IsKeyboardKeyDown(Keybindings::D_PAD_LEFT))
	{
		m_DirFacing = Direction::LEFT;
		if (m_AnimationState != AnimationState::JUMPING &&
			m_AnimationState != AnimationState::SPIN_JUMPING &&
			m_AnimationState != AnimationState::FALLING &&
			!m_IsDucking &&
			!m_IsRunning)
		{
			m_AnimationState = AnimationState::WALKING;
		}
		targetXVel = maxXVel * m_DirFacing;
	}
	else if (GAME_ENGINE->IsKeyboardKeyDown(Keybindings::D_PAD_RIGHT))
	{
		m_DirFacing = Direction::RIGHT;
		if (m_AnimationState != AnimationState::JUMPING &&
			m_AnimationState != AnimationState::SPIN_JUMPING &&
			m_AnimationState != AnimationState::FALLING &&
			!m_IsDucking &&
			!m_IsRunning)
		{
			m_AnimationState = AnimationState::WALKING;
		}
		targetXVel = maxXVel * m_DirFacing;
	}

	if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::Y_BUTTON) || GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::X_BUTTON))
	{
		if (m_IsRidingYoshi)
		{
			m_RidingYoshiPtr->SpitOutItem();

			if (m_RidingYoshiPtr->IsTongueStuckOut() == false)
			{
				m_RidingYoshiPtr->StickTongueOut(deltaTime);
			}
		}
		else if (m_IsHoldingItem == false)
		{
			// Grab blocks
			if (m_RecentlyTouchedGrabBlocksPtrArr[0] != nullptr)
			{
				if (m_RecentlyTouchedGrabBlocksPtrArr[0]->GetType() == Item::Type::GRAB_BLOCK) // Prevent strange crash
				{
					AddItemToBeHeld(m_RecentlyTouchedGrabBlocksPtrArr[0]);
					m_RecentlyTouchedGrabBlocksPtrArr[0] = nullptr;

					if (m_RecentlyTouchedGrabBlocksPtrArr[1] != nullptr)
					{
						m_RecentlyTouchedGrabBlocksPtrArr[0] = m_RecentlyTouchedGrabBlocksPtrArr[1];
						m_RecentlyTouchedGrabBlocksPtrArr[1] = nullptr;
					}

					GrabBlock* grabBlockPtr = (GrabBlock*)m_ItemHoldingPtr;
					grabBlockPtr->Grab();
				}
			}
		}
	}

	if (targetXVel != 0.0)
	{
		if (GAME_ENGINE->IsKeyboardKeyDown(Keybindings::Y_BUTTON) || 
			GAME_ENGINE->IsKeyboardKeyDown(Keybindings::X_BUTTON)) // Running
		{
			if (!m_IsDucking)
			{
				if (m_IsOnGround &&
					m_AnimationState != AnimationState::SPIN_JUMPING &&
					m_AnimationState != AnimationState::JUMPING)
				{
					m_AnimationState = AnimationState::WALKING;
					m_IsRunning = true;
				}
				
				maxXVel = MAX_RUN_VEL;
				targetXVel = maxXVel * m_DirFacing;
			}
		}
		else
		{
			m_IsRunning = false;
		}
	}
	else
	{
		if (m_IsRunning &&
			GAME_ENGINE->IsKeyboardKeyDown(Keybindings::Y_BUTTON) == false && 
			GAME_ENGINE->IsKeyboardKeyDown(Keybindings::X_BUTTON) == false)
		{
			m_IsRunning = false;
		}
	}

	// Only look up if we aren't moving horizontally or vertically
	if (GAME_ENGINE->IsKeyboardKeyDown(Keybindings::D_PAD_UP))
	{
		if (m_IsOverlappingWithBeanstalk)
		{
			SetClimbingBeanstalk(true);
			return;
		}
		else if (abs(m_ActPtr->GetLinearVelocity().x) < 0.01 &&
				 abs(m_ActPtr->GetLinearVelocity().y) < 0.01)
		{
			m_AnimationState = AnimationState::WAITING;
			m_IsLookingUp = true;
		}
	}
	else if (GAME_ENGINE->IsKeyboardKeyDown(Keybindings::D_PAD_DOWN))
	{
		if (m_IsOverlappingWithBeanstalk && !m_IsOnGround)
		{
			SetClimbingBeanstalk(true);
			return;
		}
		else if (m_IsOnGround)
		{
			m_AnimationState = AnimationState::WAITING;
			targetXVel = 0.0;
		}

		m_IsDucking = true;
	}

	if (targetXVel == 0.0 &&
		targetYVel == 0.0 && 
		abs(m_ActPtr->GetLinearVelocity().x) < 0.01 &&
		abs(m_ActPtr->GetLinearVelocity().y) < 0.01)
	{
		if (!m_IsDucking &&
			!m_IsLookingUp &&
			m_AnimationState != AnimationState::JUMPING &&
			m_AnimationState != AnimationState::SPIN_JUMPING)
		{
			m_AnimationState = AnimationState::WAITING;
		}
	}

	DOUBLE2 prevVel = m_ActPtr->GetLinearVelocity();
	double newXVel = 0.0;
	if (targetXVel != 0.0)
	{
		double horizontalAcceleration;
		if (m_IsRunning && abs(m_ActPtr->GetLinearVelocity().x) > MAX_WALK_VEL)
			horizontalAcceleration = RUNNING_ACCELERATION * deltaTime;
		else horizontalAcceleration = WALKING_ACCELERATION * deltaTime;

		double deltaXVel = horizontalAcceleration * deltaTime;
		if (prevVel.x > targetXVel)
		{
			deltaXVel = -deltaXVel;
			if (deltaXVel < targetXVel) newXVel = targetXVel; // Don't overshoot the target vel
			if (prevVel.x + deltaXVel < -maxXVel) newXVel = -maxXVel; // don't overshoot the max
			else newXVel = prevVel.x + deltaXVel;
		}
		else if (prevVel.x < targetXVel)
		{
			if (deltaXVel > targetXVel) newXVel = targetXVel;
			if (prevVel.x + deltaXVel > maxXVel) newXVel = maxXVel;
			else newXVel = prevVel.x + deltaXVel;
		}
	}

	if (targetYVel == 0.0) targetYVel = prevVel.y;
	if (targetYVel > MAX_FALL_VEL)  targetYVel = MAX_FALL_VEL;

	if (newXVel == 0.0)
	{
		newXVel = prevVel.x;
	}

	DOUBLE2 newVel = DOUBLE2(newXVel, targetYVel);
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
		m_ItemHoldingPtr->SetLinearVelocity(DOUBLE2(0.0, 0.0));
	}

	// Dust particles / changing directions
	bool dirSwitched = m_DirFacingLastFrame != m_DirFacing;
	bool duckSliding = (m_IsDucking && abs(m_ActPtr->GetLinearVelocity().x) > 1);
	if (dirSwitched)
	{
		m_ChangingDirectionsTimer.Start();
	}

	if (dirSwitched || duckSliding)
	{
		if (m_SpawnDustCloudTimer.IsActive() == false && m_IsOnGround)
		{
			DustParticle* dustParticlePtr = new DustParticle(m_ActPtr->GetPosition() + DOUBLE2(0, GetHeight() / 2 + 1));
			m_LevelPtr->AddParticle(dustParticlePtr);
			m_SpawnDustCloudTimer.Start();
		}
	}

	m_WasOnGround = m_IsOnGround;
	m_DirFacingLastFrame = m_DirFacing;
}

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

void Player::Paint()
{
    const MATRIX3X2 matPrevWorld = GAME_ENGINE->GetWorldMatrix();

	if (m_ExtraItemPtr != nullptr)
	{
		m_ExtraItemPtr->Paint();
	}

	// The player flashes while invincible
	if (m_InvincibilityTimer.IsActive()) 
	{
		const int slowFlashStart = 50;
		const int fastFlashStart = m_InvincibilityTimer.TotalFrames() - 50;

		const bool slowFlash = m_InvincibilityTimer.FramesElapsed() >= slowFlashStart &&
							   m_InvincibilityTimer.FramesElapsed() < fastFlashStart &&
							   m_InvincibilityTimer.FramesElapsed() % 10 < 5;

		const bool fastFlash = m_InvincibilityTimer.FramesElapsed() >= fastFlashStart &&
							   m_InvincibilityTimer.FramesElapsed() % 6 < 3;

		if (slowFlash || fastFlash)
		{
			return;
		}
	}

	const double centerX = m_ActPtr->GetPosition().x;
	const double centerY = m_ActPtr->GetPosition().y;

	const bool climbingFlip = m_AnimationState == AnimationState::CLIMBING && m_LastClimbingPose == Direction::LEFT;

	bool changingDirections = m_ChangingDirectionsTimer.IsActive();
	bool firstHalf = m_ChangingDirectionsTimer.FramesElapsed() < m_ChangingDirectionsTimer.TotalFrames() / 2;
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

	SpriteSheet* spriteSheetToPaint = GetSpriteSheetForPowerupState(m_PowerupState);
	INT2 spriteTile(-1, -1);
	double yo;
	if (m_PowerupTransitionTimer.IsActive())
	{
		if (m_PowerupTransitionTimer.FramesElapsed() == 1)
		{
 			m_LevelPtr->SetPausedTimer(m_PowerupTransitionTimer.TotalFrames()-1, false);
		}

		yo = double(GetHeight()) / 2.0 - (m_PowerupState == PowerupState::NORMAL ? 6 : 4);

		if (m_PowerupTransitionTimer.FramesElapsed() < m_PowerupTransitionTimer.TotalFrames() / 2)
		{
			if (m_PowerupTransitionTimer.FramesElapsed() % 8 < 4)
			{
				spriteSheetToPaint = GetSpriteSheetForPowerupState(m_PrevPowerupState);
			} 
			else
			{
				// "Medium" mario
				spriteSheetToPaint = SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::SMALL_MARIO);
				spriteTile = INT2(7, 1);
			}
		}
		else // in the second half of the transition
		{
			if (m_PowerupTransitionTimer.FramesElapsed() % 8 < 4)
			{
				spriteSheetToPaint = GetSpriteSheetForPowerupState(m_PowerupState);
			}
			else
			{
				// "Medium" mario
				spriteSheetToPaint = SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::SMALL_MARIO);
				spriteTile = INT2(7, 1);
			}
		}
	}
	else
	{
		if (m_IsRidingYoshi)
		{
			yo = GetHeight() / 2.0 - 9;
		}
		else if (m_PowerupState == PowerupState::NORMAL)
		{
			yo = GetHeight() / 2.0 - 6;
		}
		else
		{ 
			yo = GetHeight() / 2.0 - 2;
		}
	}

	if (spriteTile.x == -1 || spriteTile.y == -1)
	{
		spriteTile = CalculateAnimationFrame();
	}
	spriteSheetToPaint->Paint(centerX, centerY - GetHeight() / 2 + yo, spriteTile.x, spriteTile.y);

	// Yoshi's Tongue
	if (m_IsRidingYoshi && m_RidingYoshiPtr->IsTongueStuckOut())
	{
		int toungeTimerFramesElapsed = m_RidingYoshiPtr->GetTongueTimer().FramesElapsed();
		int toungeTimerTotalFrames = m_RidingYoshiPtr->GetTongueTimer().TotalFrames();
		
		if (toungeTimerFramesElapsed > 6 &&
			toungeTimerFramesElapsed < toungeTimerTotalFrames - 7)
		{
			SpriteSheet* yoshiWithMarioBmpPtr = SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::YOSHI_WITH_MARIO);
			int spriteTileWidth = yoshiWithMarioBmpPtr->GetTileWidth();
			int spriteTileHeight = yoshiWithMarioBmpPtr->GetTileHeight();
			int srcX = 13 * spriteTileWidth;
			int srcY = 0 * spriteTileHeight;
			int width = int(m_RidingYoshiPtr->GetTongueLength() - 7);
			int height = spriteTileHeight;
			int left = int(centerX - m_RidingYoshiPtr->GetWidth() / 2 + 32 - spriteTileWidth / 2);
			int top = int(centerY - m_RidingYoshiPtr->GetHeight() / 2 - 1 - spriteTileHeight / 2);
			RECT2 srcRect(srcX, srcY, srcX + width, srcY + height);
			// NOTE: We can't use the normal sprite sheet paint here since
			// we need to draw only part of yoshi's tounge
			GAME_ENGINE->DrawBitmap(yoshiWithMarioBmpPtr->GetBitmap(), left, top, srcRect);

			// Tongue tip
			left += width - 3;
			top += spriteTileHeight / 2 + 7;
			srcX = 455;
			srcY = 26;
			width = 7;
			height = 6;
			srcRect = RECT2(srcX, srcY, srcX + width, srcY + height);
			GAME_ENGINE->DrawBitmap(yoshiWithMarioBmpPtr->GetBitmap(), left, top, srcRect);
		}

		// The last 7 frames are when the item is in yoshi's mouth, don't paint it then
		GAME_ENGINE->SetWorldMatrix(matPrevWorld);
		if (m_RidingYoshiPtr->GetTongueTimer().FramesRemaining() > 7) m_RidingYoshiPtr->PaintEntityOnTongue();
	}

	GAME_ENGINE->SetWorldMatrix(matPrevWorld);
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
		case AnimationState::WAITING:
		case AnimationState::JUMPING:
		case AnimationState::FALLING:
		case AnimationState::CLIMBING:
		case AnimationState::IN_PIPE:
		{
			if (m_IsRidingYoshi && m_RidingYoshiPtr->IsTongueStuckOut())
			{
				if (m_AnimInfo.frameNumber > 3) m_AnimInfo.frameNumber = 3;
			}
			else
			{
				// NOTE: 1 frame animation
				m_AnimInfo.frameNumber = 0;
			}
		} break;
		case AnimationState::WALKING:
		{
			if (m_IsRidingYoshi)
			{
				if (m_RidingYoshiPtr->IsTongueStuckOut())
				{
					if (m_AnimInfo.frameNumber > 3) m_AnimInfo.frameNumber = 3;
				}
				else
				{
					m_AnimInfo.frameNumber %= 2;
				}
			}
			else if (m_ChangingDirectionsTimer.IsActive())
			{
				m_AnimInfo.frameNumber = 0;
			}
			else
			{
				if (m_PowerupState == PowerupState::NORMAL)
				{
					m_AnimInfo.frameNumber %= 2;
				}
				else if (m_PowerupState == PowerupState::SUPER)
				{
					m_AnimInfo.frameNumber %= 3;
				}
			}
		} break;
		case AnimationState::SPIN_JUMPING:
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
		else
		{
			srcY = 1;

			if (m_AnimationState == AnimationState::JUMPING) 
			{
				if (m_PowerupState == PowerupState::NORMAL) srcX = 3;
				else if (m_PowerupState == PowerupState::SUPER) srcX = 4;
			}
			else if (m_AnimationState == AnimationState::WAITING) srcX = 2;
			else srcX = 2 + m_AnimInfo.frameNumber;
		}
	}
	else if (m_IsRidingYoshi)
	{
		bool playerIsSmall = m_PowerupState == PowerupState::NORMAL;
		srcY = playerIsSmall ? 0 : 1;

		if (m_RidingYoshiPtr->IsTongueStuckOut())
		{
			srcX = 9 + m_AnimInfo.frameNumber;
		}
		else if (m_ChangingDirectionsTimer.IsActive())
		{
			srcX = 4;
		}
		else
		{
			switch (m_AnimationState)
			{
			case AnimationState::WAITING:
			{
				if (m_IsDucking)
				{
					if (m_RidingYoshiPtr->IsItemInMouth())
					{
						srcX = 6;
					}
					else
					{
						srcX = 5;
					}
				}
				else
				{
					if (m_RidingYoshiPtr->IsItemInMouth())
					{
						srcX = 7;
					}
					else
					{
						srcX = 1;
					}
				}
			} break;
			case AnimationState::WALKING:
			{
				if (m_RidingYoshiPtr->IsItemInMouth())
				{
					srcX = 7 + m_AnimInfo.frameNumber;
				}
				else
				{
					srcX = 1 + m_AnimInfo.frameNumber;
				}
			} break;
			case AnimationState::JUMPING:
			{
				srcX = 3;
			} break;
			case AnimationState::FALLING:
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
		case AnimationState::WAITING:
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
		case AnimationState::WALKING:
		{
			if (m_ItemKickAnimationTimer.IsActive())
			{
				srcX = 5;
				srcY = 1;
			}
			else if (m_ChangingDirectionsTimer.IsActive())
			{
				if (m_PowerupState == PowerupState::NORMAL)
				{
					srcX = 6;
					srcY = 0;
				}
				else if (m_PowerupState == PowerupState::SUPER)
				{
					srcX = 7;
					srcY = 1;
				}
			}
			else if (IsSprinting())
			{
				if (m_PowerupState == PowerupState::NORMAL)
				{
					srcX = 4 + m_AnimInfo.frameNumber;
					srcY = 0;
				}
				else if (m_PowerupState == PowerupState::SUPER)
				{
					srcX = 5 + m_AnimInfo.frameNumber;
					srcY = 0;
				}
			}
			else
			{
				srcX = 2 + m_AnimInfo.frameNumber;
				srcY = 0;
			}
		} break;
		case AnimationState::JUMPING:
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
		case AnimationState::SPIN_JUMPING:
		{
			srcX = 3 + m_AnimInfo.frameNumber;
			srcY = 2;
		} break;
		case AnimationState::FALLING:
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
		case AnimationState::CLIMBING:
		{
			if (m_PowerupState == PowerupState::NORMAL)
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
		case AnimationState::IN_PIPE:
		{
			srcX = 3;
			srcY = 2;
		} break;
		}
	}

	return INT2(srcX, srcY);
}

void Player::RideYoshi(Yoshi* yoshiPtr)
{
	assert(m_IsHoldingItem == false);
	assert(m_RidingYoshiPtr == nullptr);

	SoundManager::PlaySoundEffect(SoundManager::Sound::YOSHI_PLAYER_MOUNT);

	m_RidingYoshiPtr = yoshiPtr;
	m_RidingYoshiPtr->SetCarryingPlayer(true, this);

	m_NeedsNewFixture = true;

	m_ChangingDirectionsTimer = SMWTimer(YOSHI_TURN_AROUND_FRAMES);
	m_AnimInfo.secondsPerFrame = Yoshi::TONGUE_STUCK_OUT_SECONDS_PER_FRAME;

	m_IsRidingYoshi = true;
	m_AnimationState = AnimationState::WAITING;
}

void Player::DismountYoshi(bool runWild, double horizontalVel)
{
	assert(m_RidingYoshiPtr != nullptr);

	if (runWild)
	{
		m_RidingYoshiPtr->RunWild();
		m_ActPtr->SetLinearVelocity(DOUBLE2(0, -350));
	}
	else
	{
		m_RidingYoshiPtr->SetCarryingPlayer(false);
		m_ActPtr->SetPosition(m_ActPtr->GetPosition() + DOUBLE2(0, -4));
		if (horizontalVel != -1.0) m_ActPtr->SetLinearVelocity(m_ActPtr->GetLinearVelocity() + DOUBLE2(horizontalVel, 0));
	}
	m_RidingYoshiPtr = nullptr;

	m_NeedsNewFixture = true;

	m_ChangingDirectionsTimer = SMWTimer(3);
	m_AnimInfo.secondsPerFrame = MARIO_SECONDS_PER_FRAME;

	m_IsRidingYoshi = false;
}

void Player::OnItemPickup(Item* itemPtr)
{
	switch (itemPtr->GetType())
	{
	case Item::Type::COIN:
	{
		AddCoin((Coin*)itemPtr);
		Coin* coinPtr = (Coin*)itemPtr;
		if (coinPtr->HasInfiniteLifetime())
		{
			coinPtr->GenerateParticles();
		}
		else
		{
			AddScore(10, false, coinPtr->GetPosition() + DOUBLE2(5, -12));
		}
	} break;
	case Item::Type::DRAGON_COIN:
	{
		DragonCoin* dragonCoinPtr = (DragonCoin*)itemPtr;
		AddDragonCoin(dragonCoinPtr);
		dragonCoinPtr->GenerateParticles();
		AddScore(2000, false, dragonCoinPtr->GetPosition() + DOUBLE2(5, -12)); // NOTE: Not quite perfectly accurate, but enough for now
	} break;
	case Item::Type::SUPER_MUSHROOM:
	{
		AddScore(1000, false, m_ActPtr->GetPosition() + DOUBLE2(0, -12));

		switch (m_PowerupState)
		{
		case PowerupState::NORMAL:
		{
			SoundManager::PlaySoundEffect(SoundManager::Sound::PLAYER_SUPER_MUSHROOM_COLLECT);
			ChangePowerupState(PowerupState::SUPER);
		} break;
		case PowerupState::SUPER:
		case PowerupState::FIRE:
		case PowerupState::BALLOON:
		case PowerupState::CAPE:
		case PowerupState::STAR:
		{
			m_ExtraItemToBeSpawnedType = itemPtr->GetType();
		} break;
		}
	} break;
	case Item::Type::ONE_UP_MUSHROOM:
	{
		AddLife(m_ActPtr->GetPosition() + DOUBLE2(0, -10));
	} break;
	case Item::Type::CAPE_FEATHER:
	{
		ChangePowerupState(PowerupState::CAPE);
	} break;
	case Item::Type::FIRE_FLOWER:
	{
		ChangePowerupState(PowerupState::FIRE);
	} break;
	case Item::Type::POWER_BALLOON:
	{
		ChangePowerupState(PowerupState::BALLOON);
	} break;
	case Item::Type::THREE_UP_MOON:
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
	if (m_PowerupState == PowerupState::NORMAL)
	{
		m_PowerupState = PowerupState::SUPER;

		m_NeedsNewFixture = true;
	}
}

void Player::ChangePowerupState(PowerupState newPowerupState, bool isUpgrade)
{
	m_PrevPowerupState = m_PowerupState;

	m_PowerupState = newPowerupState;

	m_PowerupTransitionTimer.Start();
	m_NeedsNewFixture = true;

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
}

SpriteSheet* Player::GetSpriteSheetForPowerupState(PowerupState powerupState)
{
	if (m_IsRidingYoshi)
	{
		return SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::YOSHI_WITH_MARIO);
	}
	else
	{
		switch (powerupState)
		{
		case PowerupState::NORMAL:
			return SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::SMALL_MARIO);
		case PowerupState::SUPER:
			return SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::SUPER_MARIO);
		default:
			OutputDebugString(String("Unhandled powerup state passed to Player::GetSpriteSheetForPowerupState!\n"));
			return nullptr;
		}
	}
}

void Player::AddItemToBeHeld(Item* itemPtr)
{
	assert(m_ItemHoldingPtr == nullptr);
	assert(itemPtr != nullptr);
	
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

		m_LevelPtr->GetPlayer()->AddScore(200, true, koopaShellPtr->GetPosition() + DOUBLE2(0, -12));
	}
	koopaShellPtr->KickHorizontally(direction, wasThrown);
}

void Player::DropHeldItem()
{
	assert(m_IsHoldingItem);

	switch (m_ItemHoldingPtr->GetType())
	{
	case Item::Type::KOOPA_SHELL:
	{
		((KoopaShell*)m_ItemHoldingPtr)->ShellHit(-m_DirFacing);
	} break;
	case Item::Type::GRAB_BLOCK:
	{
		((GrabBlock*)m_ItemHoldingPtr)->SetMoving(DOUBLE2(m_DirFacing * 150, 0));
	} break;
	}
	m_ItemHoldingPtr = nullptr;
	m_IsHoldingItem = false;
	m_ItemKickAnimationTimer.Start();
}

void Player::KickHeldItem(double deltaTime, bool gently)
{
	assert(m_ItemHoldingPtr != nullptr);

	switch (m_ItemHoldingPtr->GetType())
	{
	case Item::Type::KOOPA_SHELL:
	{
		KoopaShell* koopaShellPtr = (KoopaShell*)m_ItemHoldingPtr;
		if (gently)
		{
			koopaShellPtr->SetLinearVelocity(m_ActPtr->GetLinearVelocity() + DOUBLE2(m_DirFacing * 30, 0));
		}
		else
		{
			if (GAME_ENGINE->IsKeyboardKeyDown(Keybindings::D_PAD_UP))
			{
				const double xScale = 0.5;
				const double horizontalVel = (m_ActPtr->GetLinearVelocity().x) * xScale;
				((KoopaShell*)m_ItemHoldingPtr)->KickVertically(deltaTime, horizontalVel);
			}
			else
			{
				((KoopaShell*)m_ItemHoldingPtr)->KickHorizontally(m_DirFacing, true);
			}

			m_ItemKickAnimationTimer.Start();
		}
	} break;
	case Item::Type::GRAB_BLOCK:
	{
		GrabBlock* grabBlockPtr = (GrabBlock*)m_ItemHoldingPtr;
		if (GAME_ENGINE->IsKeyboardKeyDown(Keybindings::D_PAD_UP))
		{
			const double horizontalVel = (m_ActPtr->GetLinearVelocity().x);
			grabBlockPtr->KickVertically(deltaTime, horizontalVel);
		}
		else
		{
			grabBlockPtr->SetMoving(m_ActPtr->GetLinearVelocity() + DOUBLE2(m_DirFacing * 30, 0));
		}
	} break;
	}

	m_ItemHoldingPtr = nullptr;
	m_IsHoldingItem = false;
	m_ItemKickAnimationTimer.Start();
}

void Player::SetTouchingGrabBlock(bool touching, GrabBlock* grabBlockPtr)
{
	// We're already touching two, (we're most likely touching another block through the corner of the other two
	if (touching && m_RecentlyTouchedGrabBlocksPtrArr[0] != nullptr && m_RecentlyTouchedGrabBlocksPtrArr[1] != nullptr)
	{
		return;
	}

	if (touching)
	{
		if (m_RecentlyTouchedGrabBlocksPtrArr[0] != nullptr)
		{
			m_RecentlyTouchedGrabBlocksPtrArr[1] = m_RecentlyTouchedGrabBlocksPtrArr[0];
			m_RecentlyTouchedGrabBlocksPtrArr[0] = grabBlockPtr;
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
	}
}

void Player::AddCoin(Coin* coinPtr, bool playSound, bool showParticle)
{
	m_Coins++;

	if (playSound)
	{
		SoundManager::PlaySoundEffect(SoundManager::Sound::COIN_COLLECT);
	}
	
	if (m_Coins > 99)
	{
		m_Coins = 0;
		AddLife(coinPtr->GetPosition());
		// LATER: Play sound here
	}
}

void Player::AddDragonCoin(DragonCoin* dragonCoinPtr)
{
	AddCoin(dragonCoinPtr, false, false);
	m_DragonCoins++;

	SoundManager::PlaySoundEffect(SoundManager::Sound::DRAGON_COIN_COLLECT);

	if (m_DragonCoins >= 5)
	{
		m_DragonCoins = 0;
		AddLife(dragonCoinPtr->GetPosition());
	}
}

void Player::AddLife(DOUBLE2 particlePos)
{
	m_Lives++;
	OneUpParticle* oneUpParticlePtr = new OneUpParticle(particlePos);
	m_LevelPtr->AddParticle(oneUpParticlePtr);

	SoundManager::PlaySoundEffect(SoundManager::Sound::PLAYER_ONE_UP);
}

void Player::TurnInvincible()
{
	m_InvincibilityTimer.Start();
	m_IsInvincible = true;
}

void Player::Bounce(bool horizontallyAlso, int horizontalDir)
{
	double xVel = 0.0;
	if (horizontallyAlso) 
	{
		if (horizontalDir == 0)	xVel = m_ActPtr->GetLinearVelocity().x;
		else xVel = horizontalDir * BOUNCE_VEL_HORIZONTAL;
	}
	
	m_ActPtr->SetLinearVelocity(DOUBLE2(xVel, BOUNCE_VEL));
}

void Player::Die()
{
	// TODO: In the original, lives don't decrement immediately as the player dies, 
	// but rather after the screen goes dark and right before the map is shown
	m_Lives--;
	if (m_Lives < 0)
	{
		// LATER: Trigger game over screen (in level select state)
		m_Lives = STARTING_LIVES;
	}

	m_DeathAnimationTimer.Start();
	m_AnimationState = AnimationState::WAITING;
	m_IsDead = true;

	m_ActPtr->SetSensor(true);
	
	SoundManager::SetAllSongsPaused(true);
	SoundManager::PlaySoundEffect(SoundManager::Sound::PLAYER_DEATH);
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
	case PowerupState::NORMAL:
	{
		Die();
	} break;
	case PowerupState::SUPER:
	case PowerupState::CAPE:
	case PowerupState::FIRE:
	case PowerupState::BALLOON:
	{
		SoundManager::PlaySoundEffect(SoundManager::Sound::PLAYER_DAMAGE);
		ChangePowerupState(PowerupState::NORMAL, false);
		TurnInvincible();
	} break;
	case PowerupState::STAR:
	{
		// TODO: Add star powerup state
	} break;
	default:
	{
		OutputDebugString(String("ERROR: Unhandled powerup state in Player::TakeDamage()\n"));
	} break;
	}
}

void Player::AddScore(int score, bool combo, DOUBLE2 particlePosition)
{
	if (score < 0)
	{
		OutputDebugString(String("ERROR: Negative score attempted to be added to player! ") + String(score) + String("\n"));
		return;
	}

	int scoreToAdd = score;
	if (combo)
	{
		if (m_LastScoreAdded != -1)
		{
			switch (m_LastScoreAdded)
			{
			case 200:  scoreToAdd =  400; break;
			case 400:  scoreToAdd =  800; break;
			case 800:  scoreToAdd = 1000; break;
			case 1000: scoreToAdd = 2000; break;
			case 2000: scoreToAdd = 4000; break;
			case 4000: scoreToAdd = 8000; break;
			case 8000: 
			{
				m_LastScoreAdded = score;
				AddLife(particlePosition);
			} break;
			}
		}

		m_LastScoreAdded = scoreToAdd;
		m_ScoreAddedTimer.Start();
	}

	m_Score += scoreToAdd;

	if (particlePosition != DOUBLE2())
	{
		NumberParticle* numberParticlePtr = new NumberParticle(scoreToAdd, particlePosition);
		m_LevelPtr->AddParticle(numberParticlePtr);
	}
}

void Player::AddRedStars(int numberOfRedStars)
{
	assert(numberOfRedStars > 0); // Prevent removing red stars

	m_RedStars += numberOfRedStars;
}

void Player::SetOverlappingWithBeanstalk(bool overlapping)
{
	m_IsOverlappingWithBeanstalk = overlapping;
	
	if (overlapping == false && m_AnimationState == AnimationState::CLIMBING)
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
		m_AnimationState = AnimationState::CLIMBING;
	}
	else
	{
		m_ActPtr->SetGravityScale(9.8);
		if (m_IsOnGround) m_AnimationState = AnimationState::WALKING;
		else m_AnimationState = AnimationState::FALLING;
	}

	m_FramesClimbingSinceLastFlip = FRAMES_OF_CLIMBING_ANIMATION;
}

int Player::GetWidth() const
{
	static const int SMALL_WIDTH = 9;
	static const int LARGE_WIDTH = 9;

	if (m_IsRidingYoshi)
	{
		return m_RidingYoshiPtr->GetWidth();
	}
	else if (m_PowerupState == PowerupState::NORMAL)
	{
		return SMALL_WIDTH;
	}
	else
	{
		return LARGE_WIDTH;
	}
}

int Player::GetHeight() const
{
	static const int SMALL_HEIGHT = 15;
	static const int LARGE_HEIGHT = 23;

	if (m_IsRidingYoshi)
	{
		return m_RidingYoshiPtr->GetHeight();
	}
	else if (m_PowerupState == PowerupState::NORMAL)
	{
		return SMALL_HEIGHT;
	}
	else
	{
		return LARGE_HEIGHT;
	}
}

bool Player::IsInvincible() const
{
	return m_IsInvincible;
}

Item* Player::GetHeldItemPtr()
{
	return m_ItemHoldingPtr;
}

bool Player::IsHoldingItem() const
{
	return m_IsHoldingItem;
}

Item::Type Player::GetExtraItemType() const
{
	if (m_ExtraItemPtr != nullptr)
	{
		return m_ExtraItemPtr->GetType();
	}
	else
	{
		return Item::Type::NONE;
	}
}

void Player::ReleaseExtraItem(DOUBLE2 position)
{
	m_ExtraItemToBeSpawnedType = Item::Type::NONE;

	delete m_ExtraItemPtr;

	SuperMushroom* extraSuperMushroomPtr = new SuperMushroom(position, m_LevelPtr, Direction::RIGHT);

	// TODO: Mushroom needs custom physics here to slowly fall down through the screen
	
	m_ExtraItemPtr = extraSuperMushroomPtr;
}

bool Player::AttemptToEnterPipes()
{
	if (m_TouchingPipe)
	{
		if (GAME_ENGINE->IsKeyboardKeyDown(Keybindings::D_PAD_DOWN) &&
			m_PipeTouchingPtr->GetOrientation() == Pipe::Orientation::UP &&
			m_PipeTouchingPtr->IsPlayerInPositionToEnter(this))
		{
			EnterPipe();
			return true;
		}
		else if (GAME_ENGINE->IsKeyboardKeyDown(Keybindings::D_PAD_UP) &&
			m_PipeTouchingPtr->GetOrientation() == Pipe::Orientation::DOWN &&
			m_PipeTouchingPtr->IsPlayerInPositionToEnter(this))
		{
			EnterPipe();
			return true;
		}
		else if (GAME_ENGINE->IsKeyboardKeyDown(Keybindings::D_PAD_RIGHT) &&
			m_PipeTouchingPtr->GetOrientation() == Pipe::Orientation::LEFT &&
			m_PipeTouchingPtr->IsPlayerInPositionToEnter(this))
		{
			EnterPipe();
			return true;
		}
		else if (GAME_ENGINE->IsKeyboardKeyDown(Keybindings::D_PAD_LEFT) &&
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
	SoundManager::PlaySoundEffect(SoundManager::Sound::PLAYER_DAMAGE); // Same sound as taking damage

	m_AnimationState = AnimationState::IN_PIPE;
	m_ActPtr->SetSensor(true);
	m_ActPtr->SetGravityScale(0.0);
	m_LevelPtr->SetPaused(true, false);
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
		if (m_AnimationState != AnimationState::IN_PIPE)
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
	m_AnimationState = AnimationState::IN_PIPE;
}

void Player::SetPosition(DOUBLE2 newPosition)
{
	m_ActPtr->SetPosition(newPosition);
}

Player::AnimationState Player::GetAnimationState()
{
	return m_AnimationState;
}

void Player::ResetNumberOfFramesUntilEndStompSound()
{
	m_HeadStompSoundDelayTimer.Start();
}

bool Player::IsOnGround() const
{
	return m_IsOnGround;
}

DOUBLE2 Player::GetPosition() const
{
	return m_ActPtr->GetPosition();
}

int Player::GetDirectionFacing() const
{
	return m_DirFacing;
}

DOUBLE2 Player::GetLinearVelocity() const
{
	return m_ActPtr->GetLinearVelocity();
}

void Player::SetLinearVelocity(const DOUBLE2& newLinearVelRef)
{
	m_ActPtr->SetLinearVelocity(newLinearVelRef);
}

int Player::GetLives() const
{
	return m_Lives;
}

int Player::GetCoinsCollected() const
{
	return m_Coins;
}

int Player::GetDragonCoinsCollected() const
{
	return m_DragonCoins;
}

int Player::GetRedStarsCollected() const
{
	return m_RedStars;
}

int Player::GetScore() const
{
	return m_Score;
}

bool Player::IsDead() const
{
	return m_IsDead;
}

bool Player::IsRunButtonHeldDown() const
{
	return m_IsRunning;
}

bool Player::IsSprinting() const
{
	return abs(m_ActPtr->GetLinearVelocity().x) >= (MAX_RUN_VEL - 7);
}

Player::PowerupState Player::GetPowerupState() const
{
	return m_PowerupState;
}

Player::PowerupState Player::StringToPowerupState(const std::string& powerupStateStr)
{
	if (!powerupStateStr.compare("Normal")) return PowerupState::NORMAL;
	else if (!powerupStateStr.compare("Super")) return PowerupState::SUPER;
	else if (!powerupStateStr.compare("Fire")) return PowerupState::FIRE;
	else if (!powerupStateStr.compare("Cape")) return PowerupState::CAPE;
	else if (!powerupStateStr.compare("Balloon")) return PowerupState::BALLOON;
	else if (!powerupStateStr.compare("Star")) return PowerupState::STAR;
	else return PowerupState::NONE;
}

std::string Player::PowerupStateToString(PowerupState powerupState)
{
	switch (powerupState)
	{
	case PowerupState::NORMAL: return "Normal";
	case PowerupState::SUPER: return "Super";
	case PowerupState::FIRE: return "Fire";
	case PowerupState::CAPE: return "Cape";
	case PowerupState::BALLOON: return "Balloon";
	case PowerupState::STAR: return "Star";
	default: return "INVALID STATE";
	}
}

String Player::AnimationStateToString(AnimationState state)
{
	String str;

	switch (state)
	{
	case AnimationState::WAITING:
		str = String("waiting");
		break;
	case AnimationState::WALKING:
		str = String("walking");
		break;
	case AnimationState::JUMPING:
		str = String("jumping");
		break;
	case AnimationState::SPIN_JUMPING:
		str = String("spin_jumping");
		break;
	case AnimationState::FALLING:
		str = String("falling");
		break;
	case AnimationState::CLIMBING:
		str = String("climbing");
		break;
	case AnimationState::NONE:
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

bool Player::IsRidingYoshi() const
{
	return m_IsRidingYoshi;
}

bool Player::IsAirborne() const
{
	return	m_AnimationState == AnimationState::JUMPING ||
			m_AnimationState == AnimationState::SPIN_JUMPING ||
			m_AnimationState == AnimationState::FALLING;
}

bool Player::IsTransitioningPowerups() const
{
	return m_PowerupTransitionTimer.IsActive();
}

bool Player::IsDucking() const
{
	return m_IsDucking;
}

bool Player::DEBUGIsTouchingGrabBlock() const
{
	return m_RecentlyTouchedGrabBlocksPtrArr[0] != nullptr;
}
