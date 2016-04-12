#include "stdafx.h"

#include "MontyMole.h"
#include "Game.h"
#include "SpriteSheet.h"
#include "SpriteSheetManager.h"
#include "Level.h"
#include "Player.h"
#include "SoundManager.h"

MontyMole::MontyMole(DOUBLE2& startingPos, Level* levelPtr, SPAWN_LOCATION_TYPE spawnLocationType) :
	Enemy(TYPE::MONTY_MOLE, startingPos, GetWidth(), GetHeight(), SpriteSheetManager::montyMolePtr, BodyType::DYNAMIC, levelPtr, this),
	m_SpawnLocationType(spawnLocationType)
{
	m_SpawingPosition = startingPos;
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
		if (abs(m_LevelPtr->GetPlayer()->GetPosition().x - m_SpawingPosition.x) < 50)
		{
			m_AnimationState = ANIMATION_STATE::IN_GROUND;
			
			m_FramesSpentWrigglingInTheDirt = 0;
		}

	} break;
	case ANIMATION_STATE::IN_GROUND:
	{
		// TODO: Remove this redundant check?
		if (m_FramesSpentWrigglingInTheDirt > -1)
		{
			if (++m_FramesSpentWrigglingInTheDirt > FRAMES_TO_WRIGGLE_IN_DIRT_FOR)
			{
				m_FramesSpentWrigglingInTheDirt = -1;
				m_HaveSpawnedMole = true;
				m_AnimationState = ANIMATION_STATE::JUMPING_OUT_OF_GROUND;
				m_ActPtr->SetSensor(false);
				m_ActPtr->SetActive(true);
				m_ActPtr->SetLinearVelocity(DOUBLE2(0, -350));

				// TODO: Play sound effect here
				// TODO: Add rock particle
			}
		}
	} break;
	case ANIMATION_STATE::JUMPING_OUT_OF_GROUND:
	{
		if (m_ActPtr->GetLinearVelocity().y == 0.0) // LATER: On ground
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
		}
	} break;
	case ANIMATION_STATE::WALKING:
	{
		DOUBLE2 prevVel = m_ActPtr->GetLinearVelocity();
		DOUBLE2 playerPos = m_LevelPtr->GetPlayer()->GetPosition();

		// NOTE: We're either walking towards the player, or we're walking just past them before we turn around
		bool passedTargetLeft = ((m_DirFacing == FacingDirection::LEFT && (m_ActPtr->GetPosition().x - m_TargetX) < 0.0));
		bool passedTargetRight = ((m_DirFacing == FacingDirection::RIGHT && (m_TargetX - m_ActPtr->GetPosition().x) < 0.0));
		bool targetChange = false;//(m_DirFacing == FacingDirection::RIGHT && m_TargetX < m_ActPtr->GetPosition().x) || 
						    //(m_DirFacing == FacingDirection::LEFT && m_TargetX > m_ActPtr->GetPosition().x);
		if (passedTargetLeft || passedTargetRight || targetChange)
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
				// LATER: Check if player is nearby and spawn new actor if so
			}
		}
	} break;
	default:
	{
		OutputDebugString(String("ERROR: Unhandled animation state in MontyMole::Tick!\n"));
	} break;
	}
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
		// TODO: See if creating an inverse matrix here manually would be more performant
		GAME_ENGINE->SetWorldMatrix(matTranslate.Inverse() * matReflect * matTranslate * matPrevWorld);
	}

	DOUBLE2 animationFrame = GetAnimationFrame();
	m_SpriteSheetPtr->Paint(centerX, centerY + 2, animationFrame.x, animationFrame.y);

	GAME_ENGINE->SetColor(COLOR(255, 0, 0));
	GAME_ENGINE->DrawLine(m_TargetX, m_ActPtr->GetPosition().y - 300, m_TargetX, m_ActPtr->GetPosition().y + 300);

	GAME_ENGINE->SetWorldMatrix(matPrevWorld);
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