#include "stdafx.h"

#include "CharginChuck.h"
#include "Game.h"
#include "INT2.h"
#include "Player.h"
#include "SpriteSheet.h"
#include "SpriteSheetManager.h"
#include "SoundManager.h"

#include "SplatParticle.h"

const int CharginChuck::MINIMUM_PLAYER_DISTANCE = 2 * Game::WIDTH / 3;
const double CharginChuck::TARGET_OVERSHOOT_DISTANCE = 60.0;
const double CharginChuck::RUN_VEL = 7000.0;
const double CharginChuck::JUMP_VEL = -21000.0;

CharginChuck::CharginChuck(DOUBLE2 startingPos, Level* levelPtr) :
	Enemy(Type::CHARGIN_CHUCK, startingPos, GetWidth(), GetHeight(), BodyType::DYNAMIC, levelPtr, this)
{
	m_AnimationState = AnimationState::WAITING;
	m_DirFacing = Direction::LEFT;

	m_HurtTimer = CountdownTimer(150);
	m_WaitingTimer = CountdownTimer(60);
	m_WaitingTimer.Start();

	m_AnimInfo.secondsPerFrame = 0.05;

	m_HitsRemaining = 3;
}

CharginChuck::~CharginChuck()
{
}

void CharginChuck::Tick(double deltaTime)
{
	if (abs(m_LevelPtr->GetPlayer()->GetPosition().x - m_ActPtr->GetPosition().x) > MINIMUM_PLAYER_DISTANCE) return;

	if (m_WaitingTimer.Tick() && m_WaitingTimer.IsComplete())
	{
		m_AnimationState = AnimationState::CHARGIN;
		CalculateNewTarget();
	}

	if (m_HurtTimer.Tick() && m_HurtTimer.IsComplete())
	{
		m_AnimationState = AnimationState::CHARGIN;
		CalculateNewTarget();
	}

	TickAnimations(deltaTime);

	if (m_ShouldRemoveActor)
	{
		m_LevelPtr->RemoveEnemy(this);
		return;
	}

	m_IsOnGround = CalculateOnGround();
	switch (m_AnimationState)
	{
	case AnimationState::WAITING:
	{
	} break;
	case AnimationState::CHARGIN:
	{
		UpdateVelocity(deltaTime);
	} break;
	case AnimationState::JUMPING:
	{
		if (m_IsOnGround) 
		{
			m_AnimationState = AnimationState::CHARGIN;
			CalculateNewTarget();
		}
	} break;
	case AnimationState::HURT:
	{
	} break;
	case AnimationState::DEAD:
	{
		// Delete the actor once they fall below the bottom of the screen
		if (m_ActPtr->GetPosition().y > m_LevelPtr->GetHeight() + GetHeight())
		{
			m_LevelPtr->RemoveEnemy(this);
			return;
		}
	} break;
	default:
	{
		OutputDebugString(String("ERROR: Unhandled animation state in CharginChuck::Tick!\n"));
	} break;
	}

	m_DirFacingLastFrame = m_DirFacing;
}

bool CharginChuck::CalculateOnGround()
{
	return abs(m_ActPtr->GetLinearVelocity().y) < 0.1;

	if (m_ActPtr->GetLinearVelocity().y < 0) return false;

	DOUBLE2 point1 = m_ActPtr->GetPosition();
	DOUBLE2 point2 = m_ActPtr->GetPosition() + DOUBLE2(0, (GetHeight() / 2 + 2));
	DOUBLE2 intersection, normal;
	double fraction = -1.0;
	int collisionBits = Level::LEVEL | Level::BLOCK;

	if (m_LevelPtr->Raycast(point1, point2, collisionBits, intersection, normal, fraction))
	{
		return true;
	}

	return false;
}

void CharginChuck::Paint()
{
	MATRIX3X2 matPrevWorld = GAME_ENGINE->GetWorldMatrix();

	DOUBLE2 playerPos = m_ActPtr->GetPosition();
	double centerX = playerPos.x;
	double centerY = playerPos.y;

	double xScale = 1, yScale = 1;
	if (m_DirFacing == Direction::LEFT)
	{
		xScale = -1;
	}

	if (xScale != 1 || yScale != 1)
	{
		MATRIX3X2 matReflect = MATRIX3X2::CreateScalingMatrix(DOUBLE2(xScale, yScale));
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(centerX, centerY);
		MATRIX3X2 matTranslateInverse = MATRIX3X2::CreateTranslationMatrix(-centerX, -centerY);
		GAME_ENGINE->SetWorldMatrix(matTranslateInverse * matReflect * matTranslate * matPrevWorld);
	}

	INT2 animationFrame = GetAnimationFrame();
	double yo = -2.0;
	SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::CHARGIN_CHUCK)->Paint(centerX, centerY + yo, animationFrame.x, animationFrame.y);

	GAME_ENGINE->SetWorldMatrix(matPrevWorld);

#if SMW_DISPLAY_AI_DEBUG_INFO
	GAME_ENGINE->SetColor(COLOR(240, 80, 10));
	GAME_ENGINE->DrawLine(m_TargetX, m_ActPtr->GetPosition().y - 300, m_TargetX, m_ActPtr->GetPosition().y + 300);
#endif
}

void CharginChuck::UpdateVelocity(double deltaTime)
{
	DOUBLE2 chuckPos = m_ActPtr->GetPosition();
	DOUBLE2 playerPos = m_LevelPtr->GetPlayer()->GetPosition();

	// NOTE: We're either walking towards the player, or we're walking just past them before we turn around
	bool passedTargetLeft = ((m_DirFacing == Direction::LEFT && (chuckPos.x - m_TargetX) < 0.0));
	bool passedTargetRight = ((m_DirFacing == Direction::RIGHT && (m_TargetX - chuckPos.x) < 0.0));

	if (passedTargetLeft || passedTargetRight)
	{
		TurnAround();
		return;
	}

	// The player passed our target x! We should set the x to be father past them
	bool targetChange = (m_DirFacing == Direction::LEFT && playerPos.x < m_TargetX) ||
						(m_DirFacing == Direction::RIGHT && playerPos.x > m_TargetX);

	int bufferLength = 16; // How far behind us the player can get before we stop and turn around
	bool directionChange = (m_DirFacing == Direction::LEFT && playerPos.x > chuckPos.x + bufferLength) ||
						   (m_DirFacing == Direction::RIGHT && playerPos.x < chuckPos.x - bufferLength);

	if (targetChange || directionChange)
	{
		CalculateNewTarget();
	}

	if (m_IsOnGround)
	{
		// Raycast forwards to see if we need to jump over somthing
		DOUBLE2 point1 = m_ActPtr->GetPosition();
		DOUBLE2 point2 = m_ActPtr->GetPosition() + DOUBLE2(m_DirFacing * (GetWidth() / 2 + 8), 0);
		DOUBLE2 intersection, normal;
		double fraction = -1.0;
		int collisionBits = Level::LEVEL | Level::BLOCK;

		if (m_LevelPtr->Raycast(point1, point2, collisionBits, intersection, normal, fraction))
		{
			Jump(deltaTime);
		}
	}

	// Keep walking towards the target
	double newXVel = m_DirFacing * RUN_VEL * deltaTime;

	m_ActPtr->SetLinearVelocity(DOUBLE2(newXVel, m_ActPtr->GetLinearVelocity().y));
}

void CharginChuck::TurnAround()
{
	m_DirFacing = -m_DirFacing;
	m_WaitingTimer.Start();
	m_AnimationState = AnimationState::WAITING;
	m_ActPtr->SetLinearVelocity(DOUBLE2(0.0, m_ActPtr->GetLinearVelocity().y));
}

void CharginChuck::Jump(double deltaTime)
{
	double xVel = m_ActPtr->GetLinearVelocity().x;
	
	double minXVel = 250.0;
	if (xVel > 0.0 && xVel < minXVel) xVel = minXVel;
	else if (xVel < 0.0 && xVel > -minXVel) xVel = -minXVel;
	else if (xVel == 0.0) xVel = m_DirFacing * minXVel;

	m_ActPtr->SetLinearVelocity(DOUBLE2(xVel, JUMP_VEL * deltaTime));
	m_AnimationState = AnimationState::JUMPING;
}

void CharginChuck::CalculateNewTarget()
{
	Player* playerPtr = m_LevelPtr->GetPlayer();
	int dir = (playerPtr->GetPosition().x > m_ActPtr->GetPosition().x ? Direction::RIGHT : Direction::LEFT);
	if (dir != m_DirFacing) 
	{
		TurnAround();
		return;
	}

	DOUBLE2 playerPos = playerPtr->GetPosition();
	m_TargetX = playerPos.x + TARGET_OVERSHOOT_DISTANCE * m_DirFacing;
}

void CharginChuck::HeadBonk()
{
	SplatParticle* splatParticlePtr = new SplatParticle(m_ActPtr->GetPosition());
	m_LevelPtr->AddParticle(splatParticlePtr);

	switch (m_AnimationState)
	{
	case AnimationState::WAITING:
	case AnimationState::CHARGIN:
	{
		if (TakeDamage())
		{
			SoundManager::PlaySoundEffect(SoundManager::Sound::CHARGIN_CHUCK_HEAD_BONK);

			m_AnimationState = AnimationState::HURT;
			m_HurtTimer.Start();
		
			m_LevelPtr->GetPlayer()->AddScore(800, false, m_ActPtr->GetPosition());
		}
		else
		{
			SoundManager::PlaySoundEffect(SoundManager::Sound::SHELL_KICK);
		}
	} break;
	}
}

bool CharginChuck::TakeDamage()
{
	if (--m_HitsRemaining <= 0)
	{
		m_AnimationState = AnimationState::DEAD;
		m_ActPtr->SetSensor(true);

		return false;
	}

	return true;
}

INT2 CharginChuck::GetAnimationFrame()
{
	switch (m_AnimationState)
	{
	case AnimationState::WAITING:
		return INT2(0, 0);
	case AnimationState::JUMPING:
		return INT2(0, 1);
	case AnimationState::CHARGIN:
		return INT2(1 + m_AnimInfo.frameNumber, 1);
	case AnimationState::HURT:
		return INT2(m_AnimInfo.frameNumber, 2);
	case AnimationState::DEAD:
		return INT2(0, 2);
	default:
	{
		OutputDebugString(String("ERROR: Unhandled animation state in CharginChuck::GetAnimationFrame!\n"));
		return INT2(-1, -1);
	}
	}
}

void CharginChuck::TickAnimations(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);

	switch (m_AnimationState)
	{
	case AnimationState::CHARGIN:
	{
		m_AnimInfo.frameNumber %= 2;
	} break;
	case AnimationState::WAITING:
	{
		m_AnimInfo.frameNumber %= 3;
	} break;
	case AnimationState::HURT:
	{
		m_AnimInfo.frameNumber %= 4;
	} break;
	case AnimationState::DEAD:
	case AnimationState::JUMPING:
	{
		m_AnimInfo.frameNumber = 0;
	} break;
	default:
	{
		OutputDebugString(String("ERROR: Unhandled animation state in CharginChuck::TickAnimations!\n"));
	}
	}
}

bool CharginChuck::IsRising()
{
	return m_ActPtr->GetLinearVelocity().y < 0;
}

CharginChuck::AnimationState CharginChuck::GetAnimationState()
{
	return m_AnimationState;
}

int CharginChuck::GetWidth()
{
	return WIDTH;
}

int CharginChuck::GetHeight()
{
	return HEIGHT;
}
