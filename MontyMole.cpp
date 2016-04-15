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

MontyMole::MontyMole(DOUBLE2& startingPos, Level* levelPtr, SPAWN_LOCATION_TYPE spawnLocationType) :
	Enemy(TYPE::MONTY_MOLE, startingPos, GetWidth(), GetHeight(), BodyType::DYNAMIC, levelPtr, this),
	m_SpawnLocationType(spawnLocationType), m_SpawingPosition(startingPos)
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
	if (m_ShouldBeRemoved)
	{
		m_LevelPtr->RemoveEnemy(this);
		return;
	}

	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 2;


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
			m_AnimationState = ANIMATION_STATE::JUMPING_OUT_OF_GROUND;
			m_ActPtr->SetSensor(false);
			m_ActPtr->SetActive(true);
			// TODO: Find out why moles only shoot up when the player is nearby them
			m_ActPtr->SetLinearVelocity(DOUBLE2(0, -350));
			m_FramesSpentWrigglingInTheDirt = -1;
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
			DOUBLE2 playerPos = m_LevelPtr->GetPlayer()->GetPosition();
			if (playerPos.x > m_ActPtr->GetPosition().x)
			{
				m_DirFacing = FacingDirection::RIGHT;
			}
			else
			{
				m_DirFacing = FacingDirection::LEFT;
			}

			m_TargetX = playerPos.x + m_TargetOffshoot * m_DirFacing;
			m_AnimationState = ANIMATION_STATE::WALKING;

			m_IsOnGround = true;
		}
	} break;
	case ANIMATION_STATE::WALKING:
	{
		DOUBLE2 prevVel = m_ActPtr->GetLinearVelocity();
		DOUBLE2 playerPos = m_LevelPtr->GetPlayer()->GetPosition();
		DOUBLE2 molePos = m_ActPtr->GetPosition();

		// NOTE: We're either walking towards the player, or we're walking just past them before we turn around
		bool passedTargetLeft = ((m_DirFacing == FacingDirection::LEFT && (molePos.x - m_TargetX) < 0.0));
		bool passedTargetRight = ((m_DirFacing == FacingDirection::RIGHT && (m_TargetX - molePos.x) < 0.0));

		// The player passed out target x! We should set the x to be father past them
		bool targetChange = (m_DirFacing == FacingDirection::LEFT && playerPos.x < m_TargetX) ||
							(m_DirFacing == FacingDirection::RIGHT && playerPos.x > m_TargetX);
		
		bool directionChange =	(m_DirFacing == FacingDirection::LEFT && playerPos.x > molePos.x) || 
								(m_DirFacing == FacingDirection::RIGHT && playerPos.x < molePos.x);

		m_TargetX = playerPos.x + m_TargetOffshoot * m_DirFacing;

		if (passedTargetLeft || passedTargetRight)
		{
			// Turn around
			m_DirFacing = FacingDirection::OppositeDirection(m_DirFacing);
			m_TargetX = playerPos.x + m_TargetOffshoot * m_DirFacing;
		}
		else
		{
			// Keep walking towards the target
			double newXVel = prevVel.x + m_DirFacing * WALK_VEL * deltaTime;

			newXVel = CLAMP(newXVel, -MAX_HORIZONTAL_VEL, MAX_HORIZONTAL_VEL);

			m_ActPtr->SetLinearVelocity(DOUBLE2(newXVel, prevVel.y));
		}

		if (m_IsOnGround &&
			m_DirFacing != m_DirFacingLastFrame)
		{
			DustParticle* dustParticlePtr = new DustParticle(molePos + DOUBLE2(0, GetHeight() / 2));
			m_LevelPtr->AddParticle(dustParticlePtr);
		}

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
				// TODO: Check if player is nearby and spawn new actor if so
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

void MontyMole::HeadBonk()
{
	SoundManager::PlaySoundEffect(SoundManager::SOUND::KOOPA_DEATH);

	switch (m_AnimationState)
	{
	case ANIMATION_STATE::WALKING:
	case ANIMATION_STATE::JUMPING_OUT_OF_GROUND:
	{
		m_AnimationState = ANIMATION_STATE::DEAD;
	} break;
	}
}

void MontyMole::StompKill()
{
	SoundManager::PlaySoundEffect(SoundManager::SOUND::ENEMY_HEAD_STOMP_START);
	SoundManager::PlaySoundEffect(SoundManager::SOUND::ENEMY_HEAD_STOMP_END);

	m_ShouldBeRemoved = true;
}

void MontyMole::Paint()
{
	if (m_AnimationState == ANIMATION_STATE::INVISIBLE)
	{
		return;
	}
	else
	{
		if (m_HaveSpawnedMole)
		{
			SpriteSheetManager::montyMolePtr->Paint(m_SpawingPosition.x, m_SpawingPosition.y, 5, 0);
		}
	}
	MATRIX3X2 matPrevWorld = GAME_ENGINE->GetWorldMatrix();

	double centerX = m_ActPtr->GetPosition().x;
	double centerY = m_ActPtr->GetPosition().y;

	if (m_DirFacing == FacingDirection::LEFT)
	{
		MATRIX3X2 matReflect = MATRIX3X2::CreateScalingMatrix(DOUBLE2(-1, 1));
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(centerX, centerY);
		MATRIX3X2 matTranslateInverse = MATRIX3X2::CreateTranslationMatrix(-centerX, -centerY);
		GAME_ENGINE->SetWorldMatrix(matTranslateInverse * matReflect * matTranslate * matPrevWorld);
	}

	DOUBLE2 animationFrame = GetAnimationFrame();
	SpriteSheetManager::montyMolePtr->Paint(centerX, centerY + 2, animationFrame.x, animationFrame.y);
	
	GAME_ENGINE->SetWorldMatrix(matPrevWorld);

#if 0
	GAME_ENGINE->SetColor(COLOR(255, 0, 0));
	GAME_ENGINE->DrawLine(m_TargetX, m_ActPtr->GetPosition().y - 300, m_TargetX, m_ActPtr->GetPosition().y + 300);
#endif
}

DOUBLE2 MontyMole::GetAnimationFrame()
{
	switch (m_AnimationState)
	{
	case ANIMATION_STATE::INVISIBLE:
		return DOUBLE2(-1, -1);

	case ANIMATION_STATE::IN_GROUND:
		if (m_SpawnLocationType == SPAWN_LOCATION_TYPE::GROUND)
			return DOUBLE2(7 + m_AnimInfo.frameNumber, 0);
		else
			return DOUBLE2(3 + m_AnimInfo.frameNumber, 0);

	case ANIMATION_STATE::JUMPING_OUT_OF_GROUND:
		return DOUBLE2(2, 0);

	case ANIMATION_STATE::WALKING:
		return DOUBLE2(0 + m_AnimInfo.frameNumber, 0);

	case ANIMATION_STATE::DEAD:
		return DOUBLE2(1, 0);

	default:
		OutputDebugString(String("ERROR: Unhandled animation state in MontyMole::GetAnimationFrame!\n"));
		return DOUBLE2(-1, -1);
	}
}
int MontyMole::GetWidth()
{
	return WIDTH;
}

int MontyMole::GetHeight()
{
	return HEIGHT;
}