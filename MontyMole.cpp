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
#include "NumberParticle.h"

const double MontyMole::HORIZONTAL_ACCELERATION = 800.0;
const double MontyMole::MAX_HORIZONTAL_VEL = 135.0;

MontyMole::MontyMole(DOUBLE2& startingPos, Level* levelPtr, SPAWN_LOCATION_TYPE spawnLocationType, AI_TYPE aiType) :
	Enemy(TYPE::MONTY_MOLE, startingPos + DOUBLE2(GetWidth()/2, GetHeight()/2), GetWidth(), GetHeight(), BodyType::DYNAMIC, levelPtr, this),
	m_SpawnLocationType(spawnLocationType), m_SpawingPosition(startingPos + DOUBLE2(GetWidth() / 2, GetHeight() / 2)), m_AiType(aiType)
{
	m_AnimationState = ANIMATION_STATE::INVISIBLE;
	m_ActPtr->SetSensor(true);
	m_ActPtr->SetActive(false);
}

MontyMole::~MontyMole()
{
}

void MontyMole::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 2;

	if (m_ShouldRemoveActor)
	{
		delete m_ActPtr;
		m_ActPtr = nullptr;

		m_ShouldRemoveActor = false;
	}

	switch (m_AnimationState)
	{
	case ANIMATION_STATE::INVISIBLE:
	{
		// TODO: Ensure this check is similar enough to the original's functionality
		if (abs(m_LevelPtr->GetPlayer()->GetPosition().x - m_SpawingPosition.x) < 150)
		{
			m_AnimationState = ANIMATION_STATE::IN_GROUND;
			
			m_FramesSpentWrigglingInTheDirt = 0;
		}

	} break;
	case ANIMATION_STATE::IN_GROUND:
	{
		if (++m_FramesSpentWrigglingInTheDirt > FRAMES_TO_WRIGGLE_IN_DIRT_FOR)
		{
			m_FramesSpentWrigglingInTheDirt = -1;
			m_AnimationState = ANIMATION_STATE::JUMPING_OUT_OF_GROUND;
			m_ActPtr->SetSensor(false);
			m_ActPtr->SetActive(true);
			// FIXME!!: Find out why moles only go through platforms when the player is nearby them
			m_ActPtr->SetLinearVelocity(DOUBLE2(0, -350));
			m_HaveSpawnedMole = true;

			SoundManager::PlaySoundEffect(SoundManager::SOUND::BLOCK_BREAK);
			
			BlockBreakParticle* blockBreakParticlePtr = new BlockBreakParticle(DOUBLE2(m_SpawingPosition));
			m_LevelPtr->AddParticle(blockBreakParticlePtr);
		}
	} break;
	case ANIMATION_STATE::JUMPING_OUT_OF_GROUND:
	{
		if (m_ActPtr->GetLinearVelocity().y == 0.0)
		{
			m_AnimationState = ANIMATION_STATE::WALKING;

			DOUBLE2 playerPos = m_LevelPtr->GetPlayer()->GetPosition();
			if (playerPos.x > m_ActPtr->GetPosition().x)
				m_DirFacing = FacingDirection::RIGHT;
			else
				m_DirFacing = FacingDirection::LEFT;

			m_TargetX = playerPos.x + m_TargetOffshoot * m_DirFacing;

			m_IsOnGround = true;
		}
	} break;
	case ANIMATION_STATE::WALKING:
	{
		UpdatePosition(deltaTime);
	} break;
	case ANIMATION_STATE::DEAD:
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
				if (abs(m_LevelPtr->GetPlayer()->GetPosition().x - m_SpawingPosition.x) < 150)
				{
					m_AnimationState = ANIMATION_STATE::IN_GROUND;

					m_FramesSpentWrigglingInTheDirt = 0;
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
	switch (m_AiType)
	{
	case AI_TYPE::SMART:
	{
		DOUBLE2 prevVel = m_ActPtr->GetLinearVelocity();
		DOUBLE2 molePos = m_ActPtr->GetPosition();
		DOUBLE2 playerPos = m_LevelPtr->GetPlayer()->GetPosition();

		// NOTE: We're either walking towards the player, or we're walking just past them before we turn around
		bool passedTargetLeft = ((m_DirFacing == FacingDirection::LEFT && (molePos.x - m_TargetX) < 0.0));
		bool passedTargetRight = ((m_DirFacing == FacingDirection::RIGHT && (m_TargetX - molePos.x) < 0.0));

		// The player passed out target x! We should set the x to be father past them
		bool targetChange = (m_DirFacing == FacingDirection::LEFT && playerPos.x < m_TargetX) ||
			(m_DirFacing == FacingDirection::RIGHT && playerPos.x > m_TargetX);

		bool directionChange = (m_DirFacing == FacingDirection::LEFT && playerPos.x > molePos.x) ||
			(m_DirFacing == FacingDirection::RIGHT && playerPos.x < molePos.x);

		CalculateNewTarget();

		if (passedTargetLeft || passedTargetRight)
		{
			// Turn around
			m_DirFacing = FacingDirection::OppositeDirection(m_DirFacing);
			CalculateNewTarget();
		}

		// Keep walking towards the target
		double horizontalDelta = (double(m_DirFacing) * HORIZONTAL_ACCELERATION * deltaTime);
		double newXVel = prevVel.x + horizontalDelta;

		newXVel = CLAMP(newXVel, -MAX_HORIZONTAL_VEL, MAX_HORIZONTAL_VEL);

		m_ActPtr->SetLinearVelocity(DOUBLE2(newXVel, prevVel.y));

		if (m_IsOnGround &&
			m_DirFacing != m_DirFacingLastFrame)
		{
			DustParticle* dustParticlePtr = new DustParticle(molePos + DOUBLE2(0, GetHeight() / 2));
			m_LevelPtr->AddParticle(dustParticlePtr);
		}
	} break;
	case AI_TYPE::DUMB:
	{

	} break;
	default: 
	{
		OutputDebugString(String("Unhandled AI type in MontyMole::UpdatePosition! ") + String(int(m_AiType)) + String("\n"));
	}
	}
}

void MontyMole::CalculateNewTarget()
{
	DOUBLE2 playerPos = m_LevelPtr->GetPlayer()->GetPosition();
	m_TargetX = playerPos.x + m_TargetOffshoot * m_DirFacing;
	double xScale = abs(m_ActPtr->GetLinearVelocity().x / MAX_HORIZONTAL_VEL);
	m_TargetX += ((double((rand() % 10)) / 10.0) * xScale) * 40.0 - 20.0;
}

void MontyMole::HeadBonk()
{
	SoundManager::PlaySoundEffect(SoundManager::SOUND::KOOPA_DEATH);

	switch (m_AnimationState)
	{
	case ANIMATION_STATE::WALKING:
	case ANIMATION_STATE::JUMPING_OUT_OF_GROUND:
	{
		m_AnimationState = ANIMATION_STATE::DEAD;
		m_ActPtr->SetSensor(true);
	} break;
	}
}

void MontyMole::StompKill()
{
	SoundManager::PlaySoundEffect(SoundManager::SOUND::ENEMY_HEAD_STOMP_START);
	SoundManager::PlaySoundEffect(SoundManager::SOUND::ENEMY_HEAD_STOMP_END);

	EnemyPoofParticle* poofParticlePtr = new EnemyPoofParticle(m_ActPtr->GetPosition());
	m_LevelPtr->AddParticle(poofParticlePtr);

	NumberParticle* numberParticlePtr = new NumberParticle(200, m_ActPtr->GetPosition());
	m_LevelPtr->AddParticle(numberParticlePtr);

	m_AnimationState = ANIMATION_STATE::DEAD;
	m_ShouldRemoveActor = true;
}

void MontyMole::Paint()
{
	if (m_AnimationState == ANIMATION_STATE::INVISIBLE)
	{
		return;
	}
	else
	{
		if (m_SpawnLocationType == SPAWN_LOCATION_TYPE::WALL && m_HaveSpawnedMole)
		{
			SpriteSheetManager::montyMolePtr->Paint(m_SpawingPosition.x, m_SpawingPosition.y, 5, 0);
		}
	}
	if (m_ActPtr == nullptr) return;

	MATRIX3X2 matPrevWorld = GAME_ENGINE->GetWorldMatrix();

	double centerX = m_ActPtr->GetPosition().x;
	double centerY = m_ActPtr->GetPosition().y;

	int xScale = 1, yScale = 1;
	if (m_DirFacing == FacingDirection::LEFT)
		xScale = -1;

	if (m_AnimationState == ANIMATION_STATE::DEAD)
		yScale = -1;

	if (xScale != 1 || yScale != 1)
	{
		MATRIX3X2 matReflect = MATRIX3X2::CreateScalingMatrix(DOUBLE2(xScale, yScale));
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(centerX, centerY);
		MATRIX3X2 matTranslateInverse = MATRIX3X2::CreateTranslationMatrix(-centerX, -centerY);
		GAME_ENGINE->SetWorldMatrix(matTranslateInverse * matReflect * matTranslate * matPrevWorld);
	}

	INT2 animationFrame = GetAnimationFrame();
	SpriteSheetManager::montyMolePtr->Paint(centerX, centerY + 2, animationFrame.x, animationFrame.y);
	
	GAME_ENGINE->SetWorldMatrix(matPrevWorld);

#if SMW_DISPLAY_AI_DEBUG_INFO
	GAME_ENGINE->SetColor(COLOR(255, 0, 0));
	GAME_ENGINE->DrawLine(m_TargetX, m_ActPtr->GetPosition().y - 300, m_TargetX, m_ActPtr->GetPosition().y + 300);
#endif
}

INT2 MontyMole::GetAnimationFrame()
{
	switch (m_AnimationState)
	{
	case ANIMATION_STATE::INVISIBLE:
	{
		return INT2(-1, -1);
	}
	case ANIMATION_STATE::IN_GROUND:
	{
		if (m_SpawnLocationType == SPAWN_LOCATION_TYPE::GROUND)
			return INT2(7 + m_AnimInfo.frameNumber, 0);
		else
			return INT2(3 + m_AnimInfo.frameNumber, 0);
	}
	case ANIMATION_STATE::JUMPING_OUT_OF_GROUND:
	{
		return INT2(2, 0);
	}
	case ANIMATION_STATE::WALKING:
	{
		return INT2(0 + m_AnimInfo.frameNumber, 0);
	}
	case ANIMATION_STATE::DEAD:
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

MontyMole::AI_TYPE MontyMole::StringToAIType(std::string aiTypeString)
{
	if (!aiTypeString.compare("Smart")) return AI_TYPE::SMART;
	else if(!aiTypeString.compare("Dumb")) return AI_TYPE::DUMB;
	else
	{
		OutputDebugString(String("ERROR: Unhandled ai type passed to MontyMole::StringToAIType!\n"));
		return AI_TYPE::NONE;
	}
}

MontyMole::SPAWN_LOCATION_TYPE MontyMole::StringToSpawnLocationType(std::string spawnLocationTypeString)
{
	if (!spawnLocationTypeString.compare("Ground")) return SPAWN_LOCATION_TYPE::GROUND;
	else if (!spawnLocationTypeString.compare("Wall")) return SPAWN_LOCATION_TYPE::WALL;
	else
	{
		OutputDebugString(String("ERROR: Unhandled ai type passed to MontyMole::StringToSpawnLocationType!\n"));
		return SPAWN_LOCATION_TYPE::NONE;
	}
}

void MontyMole::SetPaused(bool paused)
{
	if (m_ActPtr == nullptr) return;

	if (m_AnimationState != ANIMATION_STATE::IN_GROUND &&
		m_AnimationState != ANIMATION_STATE::INVISIBLE)
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
	return (m_AnimationState != ANIMATION_STATE::DEAD);
}
