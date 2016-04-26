#include "stdafx.h"

#include "Player.h"
#include "Game.h"
#include "Enumerations.h"
#include "SpriteSheetManager.h"

#include "SoundManager.h"
#include "DustParticle.h"
#include "NumberParticle.h"
#include "SplatParticle.h"

#include "SuperMushroom.h"
#include "Coin.h"
#include "DragonCoin.h"
#include "KoopaShell.h"
#include "Yoshi.h"

#define ENTITY_MANAGER (EntityManager::GetInstance())

// STATIC INITIALIZATIONS
const double Player::WALK_SPEED = 25.0;
const double Player::RUN_SPEED = 50.0;
const double Player::DEFAULT_GRAVITY = 0.98;

Player::Player(Level* levelPtr) : 
	Entity(DOUBLE2(), BodyType::DYNAMIC, levelPtr, ActorId::PLAYER, this)
{
	Reset();

	m_ActPtr->SetFixedRotation(true);

	// NOTE: This can't go in reset since that is called after the player dies
	m_Lives = STARTING_LIVES;
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

	m_DeathAnimation = CountdownTimer(240);
	m_ChangingDirections; // This depends on the player's vel
	m_Invincibility = CountdownTimer(12);
	m_ShellKickAnimation = CountdownTimer(10);
	m_HeadStompSoundDelay = CountdownTimer(10);
	m_PowerupTransition = CountdownTimer(50);

	m_AnimationState = ANIMATION_STATE::WAITING;
	m_IsDucking = false;
	m_IsLookingUp = false;
	m_IsRunning = false;
	m_IsHoldingItem = false;
	m_IsDead = false;
	m_IsRidingYoshi = false;

	m_PowerupState = POWERUP_STATE::NORMAL;
	m_PrevPowerupState = POWERUP_STATE::NORMAL;
	m_DirFacing = FacingDirection::RIGHT;
	m_DirFacingLastFrame = FacingDirection::RIGHT;

	m_IsOverlappingWithBeanstalk = false;
	m_FramesClimbingSinceLastFlip = 0;
	m_LastClimbingPose = FacingDirection::RIGHT;

	delete m_RidingYoshiPtr;
	m_RidingYoshiPtr = nullptr;
}

int Player::GetWidth()
{
	static const int SMALL_WIDTH = 9;
	static const int LARGE_WIDTH = 9;

	if (m_PowerupState == POWERUP_STATE::NORMAL)
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

	if (m_PowerupState == POWERUP_STATE::NORMAL)
	{
		return SMALL_HEIGHT;
	}
	else
	{
		return LARGE_HEIGHT;
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
	m_VelLastFrame = m_ActPtr->GetLinearVelocity();

	if (m_IsDead)
	{
		if (m_DeathAnimation.Tick() && m_DeathAnimation.IsComplete())
		{
			m_LevelPtr->Reset();
			return;
		}

		if (m_DeathAnimation.FramesElapsed() == 1)
		{
			// NOTE: We couldn't set active = true earlier since Box2D was locked,
			// this is the next best thing
			m_LevelPtr->SetPaused(true);
		}

		// NOTE: The player doesn't fly upward until after a short delay
		// LATER: Move all harcoded values out somewhere else
		if (m_DeathAnimation.FramesElapsed() == 40)
		{
			m_ActPtr->SetActive(true);
			m_ActPtr->SetLinearVelocity(DOUBLE2(0, -420));
		}

	}

	if (m_HeadStompSoundDelay.Tick() && m_HeadStompSoundDelay.IsComplete())
	{
		SoundManager::PlaySoundEffect(SoundManager::SOUND::ENEMY_HEAD_STOMP_END);
	}

	if (m_PowerupTransition.Tick())
	{
		if (m_PowerupTransition.FramesElapsed() == 1)
		{
			// NOTE: The player is frozen while they are transforming powerup states
			// NOTE: This call can't be moved elsewhere since lovly ol' Box2D locks up
			// during BeginContact
			SetPaused(true);
		}

		if (m_PowerupTransition.IsComplete())
		{
			SetPaused(false);
		}
		else
		{
			// NOTE: The player can't provide any input while they are transforming powerup states
			return;
		}
	}

	m_ChangingDirections.Tick();
	m_ShellKickAnimation.Tick();
	m_Invincibility.Tick();

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
		m_ActPtr->AddBoxFixture(GetWidth(), GetHeight(), 0.0, 0.15);

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

	if (m_IsOnGround && 
		m_DirFacing != m_DirFacingLastFrame)
	{
		DustParticle* dustParticlePtr = new DustParticle(m_ActPtr->GetPosition() + DOUBLE2(0, GetHeight() / 2));
		m_LevelPtr->AddParticle(dustParticlePtr);

		m_ChangingDirections.Start();
	}

	if (m_IsRidingYoshi)
	{
		//m_RidingYoshiPtr->Tick(deltaTime, m_ActPtr->GetPosition());
	}

	TickAnimations(deltaTime);

	m_DirFacingLastFrame = m_DirFacing;
}

void Player::HandleClimbingState(double deltaTime)
{
	if (GAME_ENGINE->IsKeyboardKeyPressed('Z')) // NOTE: Regular jump
	{
		SetClimbingBeanstalk(false);
		m_AnimationState = ANIMATION_STATE::JUMPING;
		SoundManager::PlaySoundEffect(SoundManager::SOUND::PLAYER_JUMP);
		m_FramesSpentInAir = 0;
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
			m_LastClimbingPose = FacingDirection::OppositeDirection(m_LastClimbingPose);
		}
	}

}

void Player::HandleKeyboardInput(double deltaTime)
{
	if (m_IsDead)
	{
		return; // NOTE: The player can't do much now...
	}

	double jumpVelocity = -16000 * deltaTime;
	double verticalVel = 0.0;

	double walkVel = 5000 * deltaTime;
	double runVel = 9500 * deltaTime;
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

	if (m_AnimationState == ANIMATION_STATE::CLIMBING)
	{
		HandleClimbingState(deltaTime);
		return;
	}

	m_IsOnGround = CalculateOnGround();
	if (m_IsOnGround)
	{
		if (m_WasOnGround == false)
		{
			m_AnimationState = ANIMATION_STATE::WAITING;
		}

		bool zIsPressed = GAME_ENGINE->IsKeyboardKeyPressed('Z');
		bool xIsPressed = GAME_ENGINE->IsKeyboardKeyPressed('X');
		if (zIsPressed || 
			(xIsPressed && m_IsHoldingItem) ||
			(xIsPressed && m_IsRidingYoshi)) // NOTE: Regular jump
		{
			m_AnimationState = ANIMATION_STATE::JUMPING;
			m_IsOnGround = false;
			SoundManager::PlaySoundEffect(SoundManager::SOUND::PLAYER_JUMP);
			m_FramesSpentInAir = 0;
			verticalVel = jumpVelocity;
		}
		else if (xIsPressed) // NOTE: Spin jump
		{
			m_AnimationState = ANIMATION_STATE::SPIN_JUMPING;
			m_IsOnGround = false;
			SoundManager::PlaySoundEffect(SoundManager::SOUND::PLAYER_SPIN_JUMP);
			m_FramesSpentInAir = 0;
			verticalVel = jumpVelocity;
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

	// TODO: Fix running to walking animation update bug
	if (GAME_ENGINE->IsKeyboardKeyDown(VK_LEFT))
	{
		m_DirFacing = FacingDirection::LEFT;
		if (m_AnimationState != ANIMATION_STATE::JUMPING &&
			m_AnimationState != ANIMATION_STATE::SPIN_JUMPING &&
			m_AnimationState != ANIMATION_STATE::FALLING &&
			!m_IsDucking &&
			!m_IsRunning)
		{
			m_AnimationState = ANIMATION_STATE::WALKING;
		}
		horizontalVel = walkVel;
	}
	else if (GAME_ENGINE->IsKeyboardKeyDown(VK_RIGHT))
	{
		m_DirFacing = FacingDirection::RIGHT;
		if (m_AnimationState != ANIMATION_STATE::JUMPING &&
			m_AnimationState != ANIMATION_STATE::SPIN_JUMPING &&
			m_AnimationState != ANIMATION_STATE::FALLING &&
			!m_IsDucking &&
			!m_IsRunning)
		{
			m_AnimationState = ANIMATION_STATE::WALKING;
		}
		horizontalVel = walkVel;
	}

	if (horizontalVel != 0.0)
	{
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
				horizontalVel = runVel;
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
		if (m_DirFacing == FacingDirection::LEFT) horizontalVel = -horizontalVel;
		newVel.x = horizontalVel;
	}
	if (verticalVel != 0.0) 
	{
		newVel.y = verticalVel;
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

	if (m_ActPtr->GetPosition().x < GetWidth())
	{
		m_ActPtr->SetLinearVelocity(DOUBLE2(0, m_ActPtr->GetLinearVelocity().y));
		m_ActPtr->SetPosition(DOUBLE2(GetWidth(), m_ActPtr->GetPosition().y));
	}

	if (m_ItemHoldingPtr != nullptr)
	{
		double offset = 12;
		if (m_ChangingDirections.IsActive()) offset = 0;
		m_ItemHoldingPtr->SetPosition(m_ActPtr->GetPosition() + DOUBLE2(offset * m_DirFacing, 0));
		m_ItemHoldingPtr->SetLinearVelocity(DOUBLE2(0, 0));
	}

	m_WasOnGround = m_IsOnGround;
}

// NOTE: Technically this isn't exactly the same as in the original, since when the player
// stands on the edge of a block they can't jump, but it's definitely close enough
bool Player::CalculateOnGround()
{
	DOUBLE2 point1 = m_ActPtr->GetPosition();
	DOUBLE2 point2 = m_ActPtr->GetPosition() + DOUBLE2(0, (GetHeight() / 2 + 3));
	DOUBLE2 intersection, normal;
	double fraction = -1.0;
	int collisionBits = Level::COLLIDE_WITH_LEVEL | Level::COLLIDE_WITH_BLOCKS;

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
		{
			// NOTE: 1 frame animation
			m_AnimInfo.frameNumber = 0;
		} break;
		case ANIMATION_STATE::WALKING:
		{
			if (m_ChangingDirections.IsActive())
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

	double centerX = m_ActPtr->GetPosition().x;
	double centerY = m_ActPtr->GetPosition().y;

	bool climbingFlip = m_AnimationState == ANIMATION_STATE::CLIMBING && m_LastClimbingPose == FacingDirection::LEFT;

	bool changingDiections = m_ChangingDirections.IsActive();
	if ((m_DirFacing == FacingDirection::LEFT && !changingDiections) ||
		(m_DirFacing == FacingDirection::RIGHT && changingDiections) || 
		climbingFlip)
	{
		MATRIX3X2 matReflect = MATRIX3X2::CreateScalingMatrix(DOUBLE2(-1, 1));
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(centerX, centerY);
		MATRIX3X2 matTranslateInverse = MATRIX3X2::CreateTranslationMatrix(-centerX, -centerY);
		GAME_ENGINE->SetWorldMatrix(matTranslateInverse * matReflect * matTranslate * matPrevWorld);
	}

	POWERUP_STATE powerupStateToPaint;
	int yo;
	if (m_PowerupTransition.IsActive())
	{
		yo = GetHeight() / 2 - (m_PrevPowerupState == POWERUP_STATE::NORMAL ? 6 : 4);
		
		if (m_PowerupTransition.FramesElapsed() % 12 > 6)
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
		yo = GetHeight() / 2 - (m_PowerupState == POWERUP_STATE::NORMAL ? 6 : 2);
	}

	SpriteSheet* spriteSheetToPaint = GetSpriteSheetForPowerupState(powerupStateToPaint);
	INT2 spriteTile = CalculateAnimationFrame();
	spriteSheetToPaint->Paint(centerX, centerY - GetHeight() / 2 + yo, spriteTile.x, spriteTile.y);

	GAME_ENGINE->SetWorldMatrix(matPrevWorld);

#if 0
	GAME_ENGINE->DrawString(AnimationStateToString(m_AnimationState), DOUBLE2(centerX + 15, centerY));
	GAME_ENGINE->DrawString(String("ovlp bean: ") + String(m_IsOverlappingWithBeanstalk ? "T" : "F"), centerX + 15, centerY - 10);
#endif
	
	if (m_ExtraItemPtr != nullptr)
	{
		m_ExtraItemPtr->Paint();
	}

}

INT2 Player::CalculateAnimationFrame()
{
	int srcX = 0, srcY = 0;

	if (m_IsDead)
	{
		srcX = 6;
		srcY = 1;
		return INT2(srcX, srcY);
	}

	// NOTE: Since when holding an item behaviour is similar across all states
	// it might as well be calculated all in one place
	if (m_IsHoldingItem)
	{
		if (m_IsDucking)
		{
			srcX = 1;
			srcY = 1;
		}
		else if (m_ChangingDirections.IsActive())
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

		return INT2(srcX, srcY);
	}

	switch (m_AnimationState)
	{
	case ANIMATION_STATE::WAITING:
	{
		if (m_IsDead)
		{
			srcX = 6;
			srcY = 1;
		}
		else if (m_IsRidingYoshi)
		{
			if (m_IsDucking) srcX = 5;
			else srcX = 1;

			if (m_PowerupState == POWERUP_STATE::NORMAL) srcY = 0;
			else srcY = 1;
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
		if (m_IsRidingYoshi)
		{
			srcX = 1 + m_AnimInfo.frameNumber;
			if (m_PowerupState == POWERUP_STATE::NORMAL) srcY = 0;
			else srcY = 1;
		}
		else if (m_ShellKickAnimation.IsActive())
		{
			srcX = 5;
			srcY = 1;
		}
		else if (m_ChangingDirections.IsActive())
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
		if (m_IsRidingYoshi)
		{
			srcX = 3;
			if (m_PowerupState == POWERUP_STATE::NORMAL) srcY = 0;
			else srcY = 1;
		}
		else if (m_IsDucking)
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
		if (m_IsRidingYoshi)
		{
			srcX = 2;
			if (m_PowerupState == POWERUP_STATE::NORMAL) srcY = 0;
			else srcY = 1;
		}
		else if (m_IsDucking)
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
		srcX = 7;
		srcY = 0;
	} break;
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

	m_RidingYoshiPtr = yoshiPtr;
	m_RidingYoshiPtr->SetCarryingMario(true, this);

	m_IsRidingYoshi = true;
	m_AnimationState = ANIMATION_STATE::WAITING;
}

void Player::DismountYoshi()
{
	assert(m_RidingYoshiPtr != nullptr);
	m_RidingYoshiPtr->SetCarryingMario(false);
	m_RidingYoshiPtr = nullptr;

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

		m_Score += 10;
	} break;
	case Item::TYPE::DRAGON_COIN:
	{
		AddDragonCoin(levelPtr);
		m_Score += 2000; // NOTE: Not quite perfectly accurate, but enough for now
		((DragonCoin*)itemPtr)->GenerateParticles();
	} break;
	case Item::TYPE::SUPER_MUSHROOM:
	{
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

	m_PowerupTransition.Start();

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

void Player::AddItemToBeHeld(Item* itemPtr)
{
	assert(m_ItemHoldingPtr == nullptr);

	m_ItemHoldingPtr = itemPtr;
	m_IsHoldingItem = true;
}

void Player::KickShell(KoopaShell* koopaShellPtr)
{
	koopaShellPtr->KickHorizontally(m_DirFacing, true);
}

void Player::DropHeldItem()
{
	assert(m_IsHoldingItem);
	// LATER: Add other item type handling here
	assert(m_ItemHoldingPtr->GetType() == Item::TYPE::KOOPA_SHELL);

	((KoopaShell*)m_ItemHoldingPtr)->ShellHit();
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
			double horizontalVel = RUN_SPEED * m_DirFacing;
			if (m_ActPtr->GetLinearVelocity().x == 0.0) horizontalVel = 0.0;
			((KoopaShell*)m_ItemHoldingPtr)->KickVertically(horizontalVel);
		}
		else
		{
			((KoopaShell*)m_ItemHoldingPtr)->KickHorizontally(m_DirFacing, true);
		}

		m_ShellKickAnimation.Start();
	}

	m_ItemHoldingPtr = nullptr;

	m_IsHoldingItem = false;
	m_ShellKickAnimation.Start();
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

	m_DeathAnimation.Start();
	m_AnimationState = ANIMATION_STATE::WAITING;
	m_IsDead = true;

	m_ActPtr->SetSensor(true);
	
	SoundManager::SetAllSongsPaused(true);
	SoundManager::PlaySoundEffect(SoundManager::SOUND::PLAYER_DEATH);
}

void Player::TakeDamage()
{
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
		ChangePowerupState(POWERUP_STATE::NORMAL, false);
		m_Invincibility.Start();
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

	SuperMushroom* extraSuperMushroomPtr = new SuperMushroom(position, m_LevelPtr, FacingDirection::RIGHT);

	// TODO: FIXME: Mushroom needs custom physics here to slowly fall down through the screen
	
	m_ExtraItemPtr = extraSuperMushroomPtr;
}

Player::ANIMATION_STATE Player::GetAnimationState()
{
	return m_AnimationState;
}

void Player::ResetNumberOfFramesUntilEndStompSound()
{
	m_HeadStompSoundDelay.Start();
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

std::string Player::GetPowerupStateString()
{
	return PowerupStateToString(m_PowerupState);
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
		str = String("s_jumping");
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
	if (m_ChangingDirections.IsActive()) str += String(" chng_dirs");
	if (m_IsDead) str += String(" dying");
	if (m_IsDucking) str += String(" ducking");
	if (m_IsLookingUp) str += String(" lkng_up");
	if (m_IsHoldingItem) str += String(" hld_item");

	return str;
}
