#include "stdafx.h"

#include "Game.h"
#include "Player.h"
#include "Enumerations.h"
#include "SpriteSheetManager.h"
#include "Entity.h"

#define ENTITY_MANAGER (EntityManager::GetInstance())

// STATIC INITIALIZATIONS
const double Player::WALK_SPEED = 25.0;
const double Player::RUN_SPEED = 50.0;

Player::Player() : Entity(DOUBLE2(), SpriteSheetManager::smallMario, BodyType::DYNAMIC, this)
{
	m_ActPtr->AddBoxFixture(WIDTH, HEIGHT, 0.0);
	m_ActPtr->SetFixedRotation(true);
	m_ActPtr->SetUserData(int(ActorId::PLAYER));

	Reset();
}

Player::~Player()
{
}

void Player::Reset()
{
	m_ActPtr->SetPosition(DOUBLE2(30, 366.6));
	m_ActPtr->SetLinearVelocity(DOUBLE2(0, 0));

	m_IsOnGround = false;

	m_AnimInfo.Reset();

	m_Score = 0;
	m_Coins = 0;
	m_Lives = 5;
	m_Stars = 0;
	m_DragonCoins = 0;

	m_PowerupState = POWERUP_STATE::NORMAL;
	m_AnimationState = ANIMATION_STATE::WAITING;
	m_DirFacing = FACING_DIRECTION::RIGHT;
	m_DirLooking = LOOKING_DIRECTION::MIDDLE;
}

RECT2 Player::GetCameraRect()
{
	RECT2 result;

	result.left = m_ActPtr->GetPosition().x - WIDTH / 2;
	result.top = m_ActPtr->GetPosition().y - HEIGHT / 2;
	result.right = result.left + WIDTH;
	result.bottom = result.top + HEIGHT;

	return result;
}

bool Player::Tick(double deltaTime, Level *levelPtr)
{
#if SMW_ENABLE_JUMP_TO
	if (GAME_ENGINE->IsKeyboardKeyPressed('O'))
	{
		m_ActPtr->SetPosition(DOUBLE2(SMW_JUMP_TO_POS_X, 365));
	}
#endif

	HandleKeyboardInput(deltaTime, levelPtr);

	if (++m_FramesOfPowerupTransitionElapsed > TOTAL_FRAMES_OF_POWERUP_TRANSITION)
	{
		m_FramesOfPowerupTransitionElapsed = -1;
	}

	TickAnimations(deltaTime);

	return false;
}

void Player::HandleKeyboardInput(double deltaTime, Level* levelPtr)
{
	if (m_AnimationState == ANIMATION_STATE::DYING)
	{
		return; // NOTE: The player can't do much now...
	}

	// TODO: Move these variables to the end of the method
	double jumpVelocity = -16000;

	// TODO: Use a variable run speed (with acceleration)
	double walkVel = 6500;
	double runVel = 10500;
	double horizontalVel = 0.0;

	m_IsOnGround = levelPtr->IsPlayerOnGround();

	if (m_IsOnGround)
	{
		if (GAME_ENGINE->IsKeyboardKeyPressed('Z')) // NOTE: Regular jump
		{
			m_AnimationState = ANIMATION_STATE::JUMPING;
			m_IsOnGround = false;
			m_ActPtr->SetLinearVelocity(DOUBLE2(m_ActPtr->GetLinearVelocity().x, jumpVelocity * deltaTime));
			m_FramesSpentInAir = 0;
		}
		else if (GAME_ENGINE->IsKeyboardKeyPressed('X')) // NOTE: Spin jump
		{
			m_AnimationState = ANIMATION_STATE::SPIN_JUMPING;
			m_IsOnGround = false;
			m_ActPtr->SetLinearVelocity(DOUBLE2(m_ActPtr->GetLinearVelocity().x, jumpVelocity * deltaTime));
			m_FramesSpentInAir = 0;
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
				OutputDebugString(m_ActPtr->GetLinearVelocity().ToString() + String("\n"));
				// NOTE: gravityScale is close to 1 at the start of the jump
				// and goes towards 0 near the apex
				double gravityScale = (m_FramesSpentInAir / 12.0) * 0.5;

				// NOTE: This ensures gravityScale is in the range [0, 0.98]
				gravityScale = max(0.0, min(0.98, gravityScale));

				m_ActPtr->SetGravityScale(gravityScale);
			}
			else
			{
				m_ActPtr->SetGravityScale(0.98);
			}
		}
		else
		{
			m_ActPtr->SetGravityScale(0.98);
			m_AnimationState = ANIMATION_STATE::FALLING;
		}
	}

	if (m_IsOnGround)
	{
		if (GAME_ENGINE->IsKeyboardKeyDown('A') || GAME_ENGINE->IsKeyboardKeyDown('S')) // Running
		{
			m_AnimationState = ANIMATION_STATE::RUNNING;
			horizontalVel = runVel;
		}
	}

	if (GAME_ENGINE->IsKeyboardKeyDown(VK_LEFT))
	{
		if (horizontalVel == 0.0)
		{
			horizontalVel = -walkVel;
		}
		else
		{
			horizontalVel = -horizontalVel;
		}
		m_ActPtr->SetLinearVelocity(DOUBLE2(horizontalVel * deltaTime, m_ActPtr->GetLinearVelocity().y));
		m_DirFacing = FACING_DIRECTION::LEFT;
		if (m_AnimationState != ANIMATION_STATE::JUMPING &&
			m_AnimationState != ANIMATION_STATE::SPIN_JUMPING &&
			horizontalVel != -runVel &&
			m_AnimationState != ANIMATION_STATE::FALLING)
		{
			m_AnimationState = ANIMATION_STATE::WALKING;
		}
	}
	else if (GAME_ENGINE->IsKeyboardKeyDown(VK_RIGHT))
	{
		if (horizontalVel == 0.0)
		{
			horizontalVel = walkVel;
		}
		m_ActPtr->SetLinearVelocity(DOUBLE2(horizontalVel * deltaTime, m_ActPtr->GetLinearVelocity().y));
		m_DirFacing = FACING_DIRECTION::RIGHT;
		if (m_AnimationState != ANIMATION_STATE::JUMPING &&
			m_AnimationState != ANIMATION_STATE::SPIN_JUMPING &&
			horizontalVel != runVel &&
			m_AnimationState != ANIMATION_STATE::FALLING)
		{
			m_AnimationState = ANIMATION_STATE::WALKING;
		}
	}

	// Only look up if we aren't moving horizontally or vertically
	if (GAME_ENGINE->IsKeyboardKeyDown(VK_UP))
	{
		if (abs(m_ActPtr->GetLinearVelocity().x) < 0.01 &&
			abs(m_ActPtr->GetLinearVelocity().y) < 0.01)
		{
			m_DirLooking = LOOKING_DIRECTION::UP;
		}
	}
	else if (GAME_ENGINE->IsKeyboardKeyDown(VK_DOWN))
	{
		if (m_AnimationState != ANIMATION_STATE::JUMPING &&
			m_AnimationState != ANIMATION_STATE::SPIN_JUMPING &&
			m_AnimationState != ANIMATION_STATE::CLIMBING)
		{
			m_DirLooking = LOOKING_DIRECTION::DOWN;
			m_AnimationState = ANIMATION_STATE::DUCKING;
		}
	}
	else
	{
		m_DirLooking = LOOKING_DIRECTION::MIDDLE;
	}

	// If we're not moving:
	if (abs(m_ActPtr->GetLinearVelocity().x) < 0.01 &&
		abs(m_ActPtr->GetLinearVelocity().y) < 0.01)
	{
		// And not ducking:
		if (m_AnimationState != ANIMATION_STATE::DUCKING)
		{
			// We're waiting
			m_AnimationState = ANIMATION_STATE::WAITING;
		}
	}
	else if (m_AnimationState == ANIMATION_STATE::FALLING)
	{
		// TODO: make this more robust
		if (horizontalVel == runVel)
		{
			m_AnimationState = ANIMATION_STATE::FAST_FALLING;
		}
		else
		{
			m_AnimationState = ANIMATION_STATE::FALLING;
		}
	}
}

void Player::TickAnimations(double deltaTime)
{
	m_AnimInfo.msThisFrame += deltaTime;
	if (m_AnimInfo.msThisFrame > m_AnimInfo.msPerFrame)
	{
		m_AnimInfo.msThisFrame -= m_AnimInfo.msPerFrame;
		m_AnimInfo.frameNumber++;

		switch (m_AnimationState)
		{
		case ANIMATION_STATE::WAITING:
		{
			// NOTE: 1 frame animation
			m_AnimInfo.frameNumber = 1;
		} break;
		case ANIMATION_STATE::WALKING:
		{
			m_AnimInfo.frameNumber %= 2;
		} break;
		case ANIMATION_STATE::RUNNING:
		{
			m_AnimInfo.frameNumber %= 2;
		} break;
		case ANIMATION_STATE::JUMPING:
		{
			// NOTE: 1 frame animation
			m_AnimInfo.frameNumber = 1;
		} break;
		case ANIMATION_STATE::SPIN_JUMPING:
		{
			m_AnimInfo.frameNumber %= 4;
		} break;
		case ANIMATION_STATE::FAST_FALLING:
		case ANIMATION_STATE::FALLING:
		{
			// NOTE: 1 frame animation
			m_AnimInfo.frameNumber = 1;
		} break;
		case ANIMATION_STATE::DUCKING:
		{
			// NOTE: 1 frame animation
			m_AnimInfo.frameNumber = 1;
		} break;
		case ANIMATION_STATE::DYING:
		{
			// NOTE: 1 frame animation
			m_AnimInfo.frameNumber = 1;
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

	if (m_DirFacing == FACING_DIRECTION::LEFT)
	{
		MATRIX3X2 matReflect = MATRIX3X2::CreateScalingMatrix(DOUBLE2(-1, 1));
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(centerX, centerY);
		// TODO: See if creating an inverse matrix here manually would be more performant
		GAME_ENGINE->SetWorldMatrix(matTranslate.Inverse() * matReflect * matTranslate * matPrevWorld);
	}

	DOUBLE2 spriteTile = CalculateAnimationFrame();
	m_SpriteSheetPtr->Paint(centerX, centerY - HEIGHT / 2 + 3, spriteTile.x, spriteTile.y);

	GAME_ENGINE->SetWorldMatrix(matPrevWorld);

	GAME_ENGINE->DrawString(AnimationStateToString(m_AnimationState), centerX + 15, centerY - 15);
}

DOUBLE2 Player::CalculateAnimationFrame()
{
	double srcX = 2;
	double srcY = 0;

	switch (m_AnimationState)
	{
	case ANIMATION_STATE::WAITING:
	{
		if (m_DirLooking == LOOKING_DIRECTION::UP)
		{
			srcX -= 2;
		}
		else if (m_DirLooking == LOOKING_DIRECTION::DOWN)
		{
			srcX -= 1;
		}
	} break;
	case ANIMATION_STATE::WALKING:
	{
		srcX += m_AnimInfo.frameNumber;
	} break;
	case ANIMATION_STATE::RUNNING:
	{
		srcX += m_AnimInfo.frameNumber + 2;
	} break;
	case ANIMATION_STATE::JUMPING:
	{
		srcX = 0;
		srcY = 2;
	} break;
	case ANIMATION_STATE::SPIN_JUMPING:
	{
		srcX = 3 + m_AnimInfo.frameNumber;
		srcY = 2;
	} break;
	case ANIMATION_STATE::FALLING:
	{
		srcX = 1;
		srcY = 2;
	} break;
	case ANIMATION_STATE::FAST_FALLING:
	{
		srcX = 2;
		srcY = 2;
	} break;
	case ANIMATION_STATE::DUCKING:
	{
		srcX -= 1;
	} break;
	case ANIMATION_STATE::DYING:
	{
		srcX = 6;
		srcY = 1;
	} break;
	}

	return DOUBLE2(srcX, srcY);
}

void Player::OnItemPickup(Item* item)
{
	switch (item->GetType())
	{
	case Item::TYPE::COIN:
	{
		AddCoin();
		m_Score += 10;
	} break;
	case Item::TYPE::DRAGON_COIN:
	{
		AddDragonCoin();
		m_Score += 2000; // NOTE: Not quite perfectly accurate, but enough for now
	} break;
	case Item::TYPE::SUPER_MUSHROOM:
	{
		m_SpriteSheetPtr = SpriteSheetManager::superMario;
		ChangePowerupState(POWERUP_STATE::SUPER);
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

void Player::ChangePowerupState(POWERUP_STATE newPowerupState, bool isUpgrade)
{
	m_PrevPowerupState = m_PowerupState;

	m_PowerupState = newPowerupState;

	m_FramesOfPowerupTransitionElapsed = 0;

	if (isUpgrade)
	{
		// TODO: Play sound effect here
		// NOTE: Sound effect changes based on what state you were in before!
	}
	else
	{
		m_SpriteSheetPtr = SpriteSheetManager::smallMario;
		// TODO: Play sound effect here
		// NOTE: Sound effect changes based on what state you were in before!
	}
}

void Player::AddCoin(bool playSound)
{
	m_Coins++;
	
	if (m_Coins > 99)
	{
		m_Coins = 0;
		AddLife();
		// LATER: Play sound here
	}
}

void Player::AddDragonCoin()
{
	AddCoin(false);
	m_DragonCoins++;

	// LATER:: Play sound here

	if (m_DragonCoins >= 5)
	{
		m_DragonCoins = 0;
		AddLife();
	}
}

void Player::AddLife()
{
	m_Lives++;
	// LATER: Play sound here
}

void Player::Die()
{
	m_Lives--;

	m_AnimationState = ANIMATION_STATE::DYING;

	// LATER: Play sound here
}

void Player::TakeDamage()
{
	switch (m_PowerupState)
	{
	case POWERUP_STATE::NORMAL:
	{
		// Die
	} break;
	case POWERUP_STATE::SUPER:
	{
		ChangePowerupState(POWERUP_STATE::NORMAL, false);
	} break;
	case POWERUP_STATE::FIRE:
	{
		ChangePowerupState(POWERUP_STATE::NORMAL, false);
	} break;
	case POWERUP_STATE::CAPE:
	{
		ChangePowerupState(POWERUP_STATE::NORMAL, false);
	} break;
	case POWERUP_STATE::BALLOON:
	{
		ChangePowerupState(POWERUP_STATE::NORMAL, false);
	} break;
	case POWERUP_STATE::STAR:
	{
		// NOTE: We're invincible! :D
	} break;
	default:
	{
		OutputDebugString(String("ERROR: Unhandled powerup state in Player::TakeDamage()\n"));
	} break;
	}
}

bool Player::IsOnGround()
{
	return m_IsOnGround;
}
DOUBLE2 Player::GetPosition()
{
	return m_ActPtr->GetPosition();
}

FACING_DIRECTION Player::GetDirectionFacing()
{
	return m_DirFacing;
}

int Player::GetWidth()
{
	return WIDTH;
}

int Player::GetHeight()
{
	return HEIGHT;
}

DOUBLE2 Player::GetLinearVelocity()
{
	return m_ActPtr->GetLinearVelocity();
}

void Player::SetLinearVelocity(const DOUBLE2& newLinearVelRef)
{
	m_ActPtr->SetLinearVelocity(newLinearVelRef);
}

void Player::TogglePaused(bool paused)
{
	m_ActPtr->SetActive(!paused);
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

Item::TYPE Player::GetExtraItemType()
{
	if (m_ExtraItemPtr != nullptr)
	{
		return m_ExtraItemPtr->GetType();
	}
	else
	{
		return Item::TYPE(-1);
	}
}

String Player::AnimationStateToString(ANIMATION_STATE state)
{
	switch (state)
	{
	case ANIMATION_STATE::WAITING:
		return String("waiting");
	case ANIMATION_STATE::WALKING:
		return String("walking");
	case ANIMATION_STATE::RUNNING:
		return String("running");
	case ANIMATION_STATE::JUMPING:
		return String("jumping");
	case ANIMATION_STATE::SPIN_JUMPING:
		return String("spin jumping");
	case ANIMATION_STATE::FALLING:
		return String("falling");
	case ANIMATION_STATE::FAST_FALLING:
		return String("fast_falling");
	case ANIMATION_STATE::DYING:
		return String("dying");
	case ANIMATION_STATE::DUCKING:
		return String("ducking");
	case ANIMATION_STATE::CLIMBING:
		return String("climbing");
	}
	return String("unknown state passed to Player::AnimationStateToString: ") + String(int(state));
}
