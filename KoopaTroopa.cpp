#include "stdafx.h"

#include "KoopaTroopa.h"
#include "KoopaShell.h"
#include "Game.h"
#include "Player.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "SoundManager.h"
#include "EnemyPoofParticle.h"
#include "NumberParticle.h"

KoopaTroopa::KoopaTroopa(DOUBLE2& startingPos, Level* levelPtr, COLOUR colour) :
	Enemy(TYPE::KOOPA_TROOPA, startingPos, GetWidth(), GetHeight(), BodyType::DYNAMIC, levelPtr, this),
	m_Color(colour)
{
	m_DirFacing = FacingDirection::LEFT;
	m_AnimInfo.secondsPerFrame = 0.14;
	m_AnimationState = ANIMATION_STATE::WALKING;
}

KoopaTroopa::~KoopaTroopa()
{
}

void KoopaTroopa::Tick(double deltaTime)
{
	if (m_ShouldAddKoopaShell)
	{
		m_ShouldAddKoopaShell = false;
		KoopaShell* koopaShellPtr = new KoopaShell(m_ActPtr->GetPosition(), m_LevelPtr, m_Color);
		koopaShellPtr->AddContactListener(m_LevelPtr);
		m_LevelPtr->AddItem(koopaShellPtr);
	}
	if (m_ShouldAddMovingUpwardKoopaShell)
	{
		m_ShouldAddMovingUpwardKoopaShell = false;
		KoopaShell* koopaShellPtr = new KoopaShell(m_ActPtr->GetPosition(), m_LevelPtr, m_Color);
		koopaShellPtr->AddContactListener(m_LevelPtr);
		koopaShellPtr->ShellHit();
		m_LevelPtr->AddItem(koopaShellPtr);
	}

	if (m_ShouldBeRemoved)
	{
		m_LevelPtr->RemoveEnemy(this);
		return;
	}

	if (m_AnimationState == ANIMATION_STATE::UPSIDEDOWN_SHELLESS)
	{
		if (m_ActPtr->GetPosition().y > Game::HEIGHT + GetHeight())
		{
			m_ShouldBeRemoved = true;
		}

		return;
	}

	if (m_FramesSpentTurningAround > -1)
	{
		if (++m_FramesSpentTurningAround > FRAMES_OF_TURNAROUND)
		{
			m_FramesSpentTurningAround = -1;
		}
	}

	if (m_FramesSpentBeingShelless > -1)
	{
		if (++m_FramesSpentBeingShelless > FRAMES_OF_BEING_SHELLESS)
		{
			m_FramesSpentBeingShelless = -1;

			ChangeAnimationState(ANIMATION_STATE::WALKING_SHELLESS);

			// LATER: Move in the direction of the player
		}
	}

	if (m_FramesSpentBeingSquashed> -1)
	{
		if (++m_FramesSpentBeingSquashed > FRAMES_OF_BEING_SQUASHED)
		{
			m_FramesSpentBeingSquashed = -1;
			
			// LATER: Add sound effect here

			 m_LevelPtr->RemoveEnemy(this);
			 return;
		}
	}

	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 2;


	// NOTE: Check if this koopa is near an obstacle, if so turn around
	DOUBLE2 point1 = m_ActPtr->GetPosition();
	DOUBLE2 point2 = m_ActPtr->GetPosition() + DOUBLE2(m_DirFacing * (GetWidth() / 2 + 2), 0);
	DOUBLE2 intersection, normal;
	double fraction = -1.0;
	int collisionBits = Level::COLLIDE_WITH_LEVEL | Level::COLLIDE_WITH_ENEMIES | Level::COLLIDE_WITH_SHELLS;

	if (m_LevelPtr->Raycast(point1, point2, collisionBits, intersection, normal, fraction))
	{
		ChangeDirections();
	}


	// NOTE: Check if this koopa about to walk off an edge, if so turn around
	point1 = m_ActPtr->GetPosition();
	point2 = m_ActPtr->GetPosition() + DOUBLE2(m_DirFacing * (GetWidth() / 2 + 2), GetHeight() / 2 + 4);
	fraction = -1.0;
	collisionBits = Level::COLLIDE_WITH_LEVEL;

	if (!m_LevelPtr->Raycast(point1, point2, collisionBits, intersection, normal, fraction))
	{
		ChangeDirections();
	}

	double xVel = WALK_VEL;
	if (m_DirFacing == FacingDirection::LEFT)
	{
		xVel = -xVel;
	}

	m_ActPtr->SetLinearVelocity(DOUBLE2(xVel, m_ActPtr->GetLinearVelocity().y));
}

void KoopaTroopa::ChangeDirections()
{
	m_DirFacing = FacingDirection::OppositeDirection(m_DirFacing);

	// NOTE: Only trigger the turn around animation if we're walking
	if (m_AnimationState == ANIMATION_STATE::WALKING)
	{
		m_FramesSpentTurningAround = 0;
	}
}

void KoopaTroopa::Paint()
{
	MATRIX3X2 matPrevWorld = GAME_ENGINE->GetWorldMatrix();

	DOUBLE2 playerPos = m_ActPtr->GetPosition();
	double centerX = playerPos.x;
	double centerY = playerPos.y;

	double xScale = 1, yScale = 1;
	if (m_FramesSpentTurningAround > -1)
	{
		if (m_FramesSpentTurningAround > 3)
		{
			xScale = m_DirFacing;
		}
		else
		{
			xScale = -m_DirFacing;
		}
	}
	else if (m_DirFacing == FacingDirection::LEFT)
	{
		xScale = -1;
	}
	if (m_AnimationState == ANIMATION_STATE::UPSIDEDOWN_SHELLESS)
	{
		yScale = -1;
		centerY += 5;
	}

	if (xScale != 1 || yScale != 1)
	{
		MATRIX3X2 matReflect = MATRIX3X2::CreateScalingMatrix(DOUBLE2(xScale, yScale));
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(centerX, centerY);
		MATRIX3X2 matTranslateInverse = MATRIX3X2::CreateTranslationMatrix(-centerX, -centerY);
		GAME_ENGINE->SetWorldMatrix(matTranslateInverse * matReflect * matTranslate * matPrevWorld);
	}

	DOUBLE2 animationFrame = DetermineAnimationFrame();
	double yo = -4.5;
	SpriteSheetManager::koopaTroopaPtr->Paint(centerX, centerY + yo, animationFrame.x, animationFrame.y);

	// TODO: Add crying particle for shelless koopas

	GAME_ENGINE->SetWorldMatrix(matPrevWorld);
}

DOUBLE2 KoopaTroopa::DetermineAnimationFrame()
{
	double row, col;

	switch (m_Color)
	{
	case COLOUR::GREEN:
		row = 0;
		break;
	case COLOUR::RED:
		row = 1;
		break;
	default:
		OutputDebugString(String("Unhandled colour in KoopaTroopa::DetermineAnimationFrame:") + String(int(m_Color)) + String("\n"));
		break;
	}

	switch (m_AnimationState)
	{
	case ANIMATION_STATE::WALKING:
	{
		if (m_FramesSpentTurningAround > -1)
		{
			col = 0;
		}
		else
		{
			col = 1 + m_AnimInfo.frameNumber;
		}
	} break;
	case ANIMATION_STATE::WALKING_SHELLESS:
	{
		col = 3 + m_AnimInfo.frameNumber;
	} break;
	case ANIMATION_STATE::UPSIDEDOWN_SHELLESS:
	{
		col = 3;
	} break;
	case ANIMATION_STATE::SHELLESS:
	{
		col = 5 + m_AnimInfo.frameNumber;
	} break;
	case ANIMATION_STATE::SQUASHED:
	{
		col = 7;
	} break;
	default:
	{
		OutputDebugString(String("ERROR: Unhandled animation state in KoopaTroopa::DetermineAnimationState:") +
			String(m_AnimationState) + String("\n"));
	} break;
	}

	return DOUBLE2(col, row);
}

void KoopaTroopa::HeadBonk()
{
	SoundManager::PlaySoundEffect(SoundManager::SOUND::KOOPA_DEATH);

	switch (m_AnimationState)
	{
	case ANIMATION_STATE::WALKING:
	{
		ChangeAnimationState(ANIMATION_STATE::SHELLESS);
		m_FramesSpentBeingShelless = 0;

		m_ShouldAddKoopaShell = true;
	} break;
	case ANIMATION_STATE::SHELLESS:
	{
		ChangeAnimationState(ANIMATION_STATE::UPSIDEDOWN_SHELLESS);

		m_ActPtr->SetSensor(true);
		m_ActPtr->SetLinearVelocity(DOUBLE2(0, -250));
	} break;
	case ANIMATION_STATE::WALKING_SHELLESS:
	{
		ChangeAnimationState(ANIMATION_STATE::SQUASHED);
		m_FramesSpentBeingSquashed = 0;
	} break;
	}
}

void KoopaTroopa::ShellHit()
{
	m_ShouldAddMovingUpwardKoopaShell = true;

	SoundManager::PlaySoundEffect(SoundManager::SOUND::KOOPA_DEATH);

	m_ShouldBeRemoved = true;
}

void KoopaTroopa::StompKill()
{
	EnemyPoofParticle* poofParticlePtr = new EnemyPoofParticle(m_ActPtr->GetPosition());
	m_LevelPtr->AddParticle(poofParticlePtr);

	NumberParticle* numberParticlePtr = new NumberParticle(200, m_ActPtr->GetPosition());
	m_LevelPtr->AddParticle(numberParticlePtr);

	SoundManager::PlaySoundEffect(SoundManager::SOUND::ENEMY_HEAD_STOMP_START);
	m_LevelPtr->GetPlayer()->ResetNumberOfFramesUntilEndStompSound();

	m_ShouldBeRemoved = true;
}

void KoopaTroopa::ChangeAnimationState(ANIMATION_STATE newAnimationState)
{

	m_AnimationState = newAnimationState;

	// LATER: Add particle?
}

KoopaTroopa::ANIMATION_STATE KoopaTroopa::GetAnimationState()
{
	return m_AnimationState;
}

int KoopaTroopa::GetWidth()
{
	return WIDTH;
}

int KoopaTroopa::GetHeight()
{
	static const int SMALL_HEIGHT = 14;
	static const int LARGE_HEIGHT = 14;

	// TODO: Remove resizable actor height functionality
	if (m_AnimationState == ANIMATION_STATE::WALKING)
	{
		return LARGE_HEIGHT;
	}
	else
	{
		return SMALL_HEIGHT;
	}
}