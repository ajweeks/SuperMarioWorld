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
	m_ActPtr->SetPosition(DOUBLE2(60, 740));
	m_ActPtr->SetLinearVelocity(DOUBLE2(0, 0));

	m_IsOnGround = false;

	m_AnimInfo.Reset();

	m_Score = 0;
	m_Coins = 0;
	m_Lives = 5;
	m_Stars = 15;
	m_DragonCoins = 0;
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
		m_ActPtr->SetPosition(DOUBLE2(SMW_JUMP_TO_POS_X, 740));
	}
#endif

	// TODO: Move these variables to the end of the method
	double jumpVelocity = -32000.0;
	// How much to add each addintional frame 'jump' is held down
	double jumpHeightBoostVelocity = -0.0001;

	// TODO: Use a variable run speed (with acceleration)
	double walkVel = 13000.0;
	double runVel = 21000.0;
	double horizontalVel = 0.0;

	m_IsOnGround = levelPtr->IsOnGround(m_ActPtr);
	//if (!m_IsOnGround)
	//{
	//	m_FramesSpentInAir++;
	//	// When the player is still rising (yv < 0) they can hold jump to go higher
	//	if (m_ActPtr->GetLinearVelocity().y < 0.0 && (GAME_ENGINE->IsKeyboardKeyDown('Z') || GAME_ENGINE->IsKeyboardKeyDown('X')))
	//	{
	//		if (m_FramesSpentInAir <= 15)
	//		{
	//			double amountOfYvToRemoveThisFrame = 1000.0 * deltaTime;
	//			m_ActPtr->SetLinearVelocity(DOUBLE2(m_ActPtr->GetLinearVelocity().x, 
	//				m_ActPtr->GetLinearVelocity().y + amountOfYvToRemoveThisFrame));
	//		}
	//	}
	//}

	if (m_IsOnGround)
	{
		if (GAME_ENGINE->IsKeyboardKeyPressed('Z')) // Regular jump
		{
			m_AnimationState = ANIMATION_STATE::JUMPING;
			m_IsOnGround = false;
			m_ActPtr->SetLinearVelocity(DOUBLE2(m_ActPtr->GetLinearVelocity().x, jumpVelocity * deltaTime));
			m_FramesSpentInAir = 0;
		}
		else if (GAME_ENGINE->IsKeyboardKeyPressed('X')) // Spin jump
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
		if (m_ActPtr->GetLinearVelocity().y > 0.01)
		{
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

	TickAnimations(deltaTime);

	return false;
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

	RECT2 srcRect = CalculateAnimationFrame();
	m_SpriteSheetPtr->Paint(centerX, centerY + 22 - HEIGHT, srcRect);

	GAME_ENGINE->SetWorldMatrix(matPrevWorld);

	GAME_ENGINE->DrawString(AnimationStateToString(m_AnimationState), centerX + 25, centerY - 25);
}

RECT2 Player::CalculateAnimationFrame()
{
	double tileWidth = m_SpriteSheetPtr->GetTileWidth();
	double tileHeight = m_SpriteSheetPtr->GetTileHeight();

	double srcX = 2 * tileWidth;
	double srcY = 0;

	switch (m_AnimationState)
	{
	case ANIMATION_STATE::WAITING:
	{
		if (m_DirLooking == LOOKING_DIRECTION::UP)
		{
			srcX -= 2 * tileWidth;
		}
		else if (m_DirLooking == LOOKING_DIRECTION::DOWN)
		{
			srcX -= tileWidth;
		}
	} break;
	case ANIMATION_STATE::WALKING:
	{
		srcX += m_AnimInfo.frameNumber * tileWidth;
	} break;
	case ANIMATION_STATE::RUNNING:
	{
		srcX += m_AnimInfo.frameNumber * tileWidth + 2 * tileWidth;
	} break;
	case ANIMATION_STATE::JUMPING:
	{
		srcX = 0;
		srcY = 2 * tileHeight;
	} break;
	case ANIMATION_STATE::SPIN_JUMPING:
	{
		srcX = 3 * tileWidth + m_AnimInfo.frameNumber * tileWidth;
		srcY = 2 * tileHeight;
	} break;
	case ANIMATION_STATE::FALLING:
	{
		srcX = tileWidth;
		srcY = 2 * tileHeight;
	} break;
	case ANIMATION_STATE::FAST_FALLING:
	{
		srcX = 2 * tileWidth;
		srcY = 2 * tileHeight;
	} break;
	case ANIMATION_STATE::DUCKING:
	{
		srcX -= tileWidth;
	} break;
	case ANIMATION_STATE::DYING:
	{
		srcX = 6 * tileWidth;
		srcY = 1 * tileHeight;
	} break;
	}

	// srcY+1 to get rid of small black line (should probably find a better way to fix that though)
	return RECT2(srcX, srcY + 1, srcX + tileWidth, srcY + tileHeight - 1);
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
	return String("unknown state: ") + String(int(state));
}
