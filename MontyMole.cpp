#include "stdafx.h"

#include "MontyMole.h"
#include "Game.h"
#include "SpriteSheet.h"
#include "SpriteSheetManager.h"
#include "Level.h"
#include "Player.h"
#include "SoundManager.h"

#include "DustParticle.h"
#include "BlockBreakParticle.h"
#include "EnemyPoofParticle.h"
#include "SplatParticle.h"

const double MontyMole::HORIZONTAL_ACCELERATION = 880.0;
const double MontyMole::MAX_HORIZONTAL_VEL = 125.0;
const double MontyMole::TARGET_OVERSHOOT_DISTANCE = 30.0;

const double MontyMole::LAUNCH_OUT_OF_GROUND_VEL = -22000.0;
const double MontyMole::JUMP_VEL = -10000.0;

MontyMole::MontyMole(DOUBLE2& startingPos, Level* levelPtr, SpawnLocationType spawnLocationType, AIType aiType) :
	Enemy(Type::MONTY_MOLE, startingPos + DOUBLE2(GetWidth()/2, GetHeight()/2), GetWidth(), GetHeight(), BodyType::DYNAMIC, levelPtr, this),
	m_SpawnLocationType(spawnLocationType), m_AiType(aiType)
{
	m_AnimationState = AnimationState::INVISIBLE;
	m_ActPtr->SetSensor(true);
	m_ActPtr->SetActive(false);

	m_FramesSpentWrigglingInDirtTimer = SMWTimer(90);
	m_SpawnDustCloudTimer = SMWTimer(4);
	m_FramesSinceLastHop = SMWTimer(60);

	if (m_AiType == AIType::DUMB) m_FramesSinceLastHop.Start();
}

MontyMole::~MontyMole()
{
}

void MontyMole::Tick(double deltaTime)
{
	if (m_ActPtr != nullptr)
	{
		bool wasActive = m_IsActive;
		Enemy::Tick(deltaTime);
		if (wasActive && m_IsActive == false)
		{
			m_ActPtr->SetPosition(m_SpawingPosition);
			m_ActPtr->SetActive(false);
			m_ActPtr->SetSensor(true);
			m_AnimationState = AnimationState::IN_GROUND;
			return;
		}
		else if (wasActive == false && m_IsActive)
		{
			m_FramesSpentWrigglingInDirtTimer.Start();
		}
	}

	if (m_IsActive == false) return;

	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 2;

	m_SpawnDustCloudTimer.Tick();

	if (m_ShouldRemoveActor)
	{
		delete m_ActPtr;
		m_ActPtr = nullptr;

		m_ShouldRemoveActor = false;
		return;
	}

	if (m_ActPtr == nullptr) return;
		
	switch (m_AnimationState)
	{
	case AnimationState::INVISIBLE:
	{
		if (abs(m_LevelPtr->GetPlayer()->GetPosition().x - m_SpawingPosition.x) < PLAYER_PROXIMITY)
		{
			m_AnimationState = AnimationState::IN_GROUND;
			m_FramesSpentWrigglingInDirtTimer.Start();
		}
	} break;
	case AnimationState::IN_GROUND:
	{
		if (m_FramesSpentWrigglingInDirtTimer.Tick() && m_FramesSpentWrigglingInDirtTimer.IsComplete())
		{
			m_AnimationState = AnimationState::JUMPING_OUT_OF_GROUND;
			m_ActPtr->SetActive(true);
			m_ActPtr->SetLinearVelocity(DOUBLE2(0, LAUNCH_OUT_OF_GROUND_VEL * deltaTime));
			m_HaveSpawnedMole = true;

			SoundManager::PlaySoundEffect(SoundManager::Sound::BLOCK_BREAK);
			
			BlockBreakParticle* blockBreakParticlePtr = new BlockBreakParticle(DOUBLE2(m_SpawingPosition));
			m_LevelPtr->AddParticle(blockBreakParticlePtr);
		}
	} break;
	case AnimationState::JUMPING_OUT_OF_GROUND:
	{
		if (m_ActPtr->IsSensor() &&
			m_ActPtr->GetLinearVelocity().y > -0.5)
		{
			m_ActPtr->SetSensor(false);
		}

		if (m_ActPtr->GetLinearVelocity().y == 0.0)
		{
			m_AnimationState = AnimationState::WALKING;

			DOUBLE2 playerPos = m_LevelPtr->GetPlayer()->GetPosition();
			if (playerPos.x > m_ActPtr->GetPosition().x) m_DirFacing = Direction::RIGHT;
			else m_DirFacing = Direction::LEFT;

			CalculateNewTarget();

			m_IsOnGround = true;
		}
	} break;
	case AnimationState::WALKING:
	{
		UpdatePosition(deltaTime);
	} break;
	case AnimationState::DEAD:
	{
		if (m_ActPtr != nullptr)
		{
			if (m_ActPtr->GetPosition().y > m_LevelPtr->GetHeight() + GetHeight())
			{
				delete m_ActPtr;
				m_ActPtr = nullptr;
			}
		}
		else
		{
			if (!m_HasBeenKilledByPlayer)
			{
				if (abs(m_LevelPtr->GetPlayer()->GetPosition().x - m_SpawingPosition.x) < PLAYER_PROXIMITY)
				{
					m_AnimationState = AnimationState::IN_GROUND;

					m_FramesSpentWrigglingInDirtTimer.Start();
				}
			}
		}
	} break;
	default:
	{
		OutputDebugString(String("ERROR: Unhandled animation state in MontyMole::Tick!\n"));
	} break;
	}

	m_DirFacingLastFrame = m_DirFacing;
}

void MontyMole::UpdatePosition(double deltaTime) 
{
	DOUBLE2 prevVel = m_ActPtr->GetLinearVelocity();
	DOUBLE2 molePos = m_ActPtr->GetPosition();

	double newXVel = prevVel.x;
	double newYVel = prevVel.y;

	switch (m_AiType)
	{
	case AIType::SMART:
	{
		DOUBLE2 playerPos = m_LevelPtr->GetPlayer()->GetPosition();

		// We've walked past our target, time to turn around
		bool passedTargetLeft = ((m_DirFacing == Direction::LEFT && (molePos.x - m_TargetX) < 0.0));
		bool passedTargetRight = ((m_DirFacing == Direction::RIGHT && (m_TargetX - molePos.x) < 0.0));
		if (passedTargetLeft || passedTargetRight)
		{
			// Turn around
			m_DirFacing = -m_DirFacing;
			CalculateNewTarget();
		}

		// The player passed our target x! We should set the x to be father past them
		bool targetChange = (m_DirFacing == Direction::LEFT && playerPos.x < m_TargetX) ||
							(m_DirFacing == Direction::RIGHT && playerPos.x > m_TargetX);
		// The player is behind us, we should recalculate our target x
		bool directionChange = (m_DirFacing == Direction::LEFT && playerPos.x > molePos.x) ||
							   (m_DirFacing == Direction::RIGHT && playerPos.x < molePos.x);

		if (targetChange || directionChange)
		{
			CalculateNewTarget();
		}
	} break;
	case AIType::DUMB:
	{
		// Just walk forward, jumping periodically, until you hit a wall, at which point turn around

		// Test if we're running into an obstacle
		DOUBLE2 point1 = m_ActPtr->GetPosition();
		DOUBLE2 point2 = m_ActPtr->GetPosition() + DOUBLE2(m_DirFacing * (GetWidth() / 2 + 2), 0);
		DOUBLE2 intersection, normal;
		double fraction = -1.0;
		int collisionBits = Level::LEVEL | Level::BLOCK;
		if (m_LevelPtr->Raycast(point1, point2, collisionBits, intersection, normal, fraction))
		{
			// Turn around
			m_DirFacing = -m_DirFacing;
			CalculateNewTarget();
		}

		// See if it's time to hop again
		if (prevVel.y == 0 &&
			m_FramesSinceLastHop.Tick() && m_FramesSinceLastHop.IsComplete())
		{
			m_FramesSinceLastHop.Start();
			newYVel = JUMP_VEL * deltaTime;
		}
	} break;
	}

	// Keep walking towards the target
	double horizontalDelta = (double(m_DirFacing) * HORIZONTAL_ACCELERATION * deltaTime);
	newXVel = prevVel.x + horizontalDelta;

	newXVel = CLAMP(newXVel, -MAX_HORIZONTAL_VEL, MAX_HORIZONTAL_VEL);

	// Dust cloud spawning
	if (m_SpawnDustCloudTimer.IsActive() == false &&
		m_IsOnGround &&
		((horizontalDelta > 0 && prevVel.x < 0) ||
			(horizontalDelta < 0 && prevVel.x > 0)))
	{
		DustParticle* dustParticlePtr = new DustParticle(m_ActPtr->GetPosition() + DOUBLE2(0, GetHeight() / 2 + 1));
		m_LevelPtr->AddParticle(dustParticlePtr);

		m_SpawnDustCloudTimer.Start();
	}

	m_ActPtr->SetLinearVelocity(DOUBLE2(newXVel, newYVel));
}

void MontyMole::CalculateNewTarget()
{
	DOUBLE2 playerPos = m_LevelPtr->GetPlayer()->GetPosition();
	m_TargetX = playerPos.x + TARGET_OVERSHOOT_DISTANCE * m_DirFacing;
	double xScale = abs(m_ActPtr->GetLinearVelocity().x / MAX_HORIZONTAL_VEL);

	// NOTE: Adds some randomness to the target to prevent several moles from walking the exact same path
	double maxVariation = 40.0;
	m_TargetX += ((double((rand() % 10)) / 9.0) * xScale) * maxVariation - maxVariation / 2;
}

void MontyMole::HeadBonk()
{
	SoundManager::PlaySoundEffect(SoundManager::Sound::SHELL_KICK);

	switch (m_AnimationState)
	{
	case AnimationState::WALKING:
	case AnimationState::JUMPING_OUT_OF_GROUND:
	{
		m_AnimationState = AnimationState::DEAD;

		m_ActPtr->SetLinearVelocity(DOUBLE2(0.0, 0.0));

		SplatParticle* splatParticlePtr = new SplatParticle(m_ActPtr->GetPosition());
		m_LevelPtr->AddParticle(splatParticlePtr);

		m_LevelPtr->GetPlayer()->AddScore(200, true, m_ActPtr->GetPosition());

		m_ActPtr->SetSensor(true);
	} break;
	}
}

void MontyMole::StompKill()
{
	SoundManager::PlaySoundEffect(SoundManager::Sound::ENEMY_HEAD_STOMP_START);
	SoundManager::PlaySoundEffect(SoundManager::Sound::ENEMY_HEAD_STOMP_END);

	EnemyPoofParticle* poofParticlePtr = new EnemyPoofParticle(m_ActPtr->GetPosition());
	m_LevelPtr->AddParticle(poofParticlePtr);

	m_LevelPtr->GetPlayer()->AddScore(200, true, m_ActPtr->GetPosition());

	m_AnimationState = AnimationState::DEAD;
	m_ShouldRemoveActor = true;
}

void MontyMole::Paint()
{
	if (m_SpawnLocationType == SpawnLocationType::WALL && m_HaveSpawnedMole)
	{
		SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::MONTY_MOLE)->Paint(m_SpawingPosition.x, m_SpawingPosition.y, 5, 0);
	}
	if (m_AnimationState == AnimationState::INVISIBLE)
	{
		return;
	}
	if (m_ActPtr == nullptr) return;

	MATRIX3X2 matPrevWorld = GAME_ENGINE->GetWorldMatrix();

	double centerX = m_ActPtr->GetPosition().x;
	double centerY = m_ActPtr->GetPosition().y;

	int xScale = 1, yScale = 1;
	if (m_DirFacing == Direction::LEFT) xScale = -1;
	if (m_AnimationState == AnimationState::DEAD) yScale = -1;

	if (xScale != 1 || yScale != 1)
	{
		MATRIX3X2 matReflect = MATRIX3X2::CreateScalingMatrix(DOUBLE2(xScale, yScale));
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(centerX, centerY);
		MATRIX3X2 matTranslateInverse = MATRIX3X2::CreateTranslationMatrix(-centerX, -centerY);
		GAME_ENGINE->SetWorldMatrix(matTranslateInverse * matReflect * matTranslate * matPrevWorld);
	}

	INT2 animationFrame = GetAnimationFrame();
	SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::MONTY_MOLE)->Paint(centerX, centerY + 1.5, animationFrame.x, animationFrame.y);
	
	GAME_ENGINE->SetWorldMatrix(matPrevWorld);

#if SMW_DISPLAY_AI_DEBUG_INFO
	GAME_ENGINE->SetColor(COLOR(235, 50, 0));
	GAME_ENGINE->DrawLine(m_TargetX, m_ActPtr->GetPosition().y - 300, m_TargetX, m_ActPtr->GetPosition().y + 300);
#endif
}

INT2 MontyMole::GetAnimationFrame()
{
	switch (m_AnimationState)
	{
	case AnimationState::INVISIBLE:
	{
		return INT2(-1, -1);
	}
	case AnimationState::IN_GROUND:
	{
		if (m_SpawnLocationType == SpawnLocationType::GROUND)
			return INT2(7 + m_AnimInfo.frameNumber, 0);
		else
			return INT2(3 + m_AnimInfo.frameNumber, 0);
	}
	case AnimationState::JUMPING_OUT_OF_GROUND:
	{
		return INT2(2, 0);
	}
	case AnimationState::WALKING:
	{
		return INT2(0 + m_AnimInfo.frameNumber, 0);
	}
	case AnimationState::DEAD:
	{
		return INT2(1, 0);
	}
	default:
	{
		OutputDebugString(String("ERROR: Unhandled animation state in MontyMole::GetAnimationFrame!\n"));
		return INT2(-1, -1);
	}
	}
}

MontyMole::AIType MontyMole::StringToAIType(std::string aiTypeString)
{
	if (!aiTypeString.compare("Smart")) return AIType::SMART;
	else if(!aiTypeString.compare("Dumb")) return AIType::DUMB;
	else
	{
		OutputDebugString(String("ERROR: Unhandled ai type passed to MontyMole::StringToAIType!\n"));
		return AIType::NONE;
	}
}

MontyMole::SpawnLocationType MontyMole::StringToSpawnLocationType(std::string spawnLocationTypeString)
{
	if (!spawnLocationTypeString.compare("Ground")) return SpawnLocationType::GROUND;
	else if (!spawnLocationTypeString.compare("Wall")) return SpawnLocationType::WALL;
	else
	{
		OutputDebugString(String("ERROR: Unhandled ai type passed to MontyMole::StringToSpawnLocationType!\n"));
		return SpawnLocationType::NONE;
	}
}

void MontyMole::SetPaused(bool paused)
{
	if (m_ActPtr == nullptr) return;

	if (m_AnimationState != AnimationState::IN_GROUND &&
		m_AnimationState != AnimationState::INVISIBLE)
	{
		m_ActPtr->SetActive(!paused);
	}
}

bool MontyMole::Raycast(DOUBLE2 point1, DOUBLE2 point2, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef)
{
	if (m_ActPtr == nullptr) return false;

	return m_ActPtr->Raycast(point1, point2, intersectionRef, normalRef, fractionRef);
}

int MontyMole::GetWidth()
{
	return WIDTH;
}

int MontyMole::GetHeight()
{
	return HEIGHT;
}

bool MontyMole::IsAlive()
{
	return (m_AnimationState != AnimationState::DEAD);
}
