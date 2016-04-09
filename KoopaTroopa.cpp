#include "stdafx.h"

#include "KoopaTroopa.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

KoopaTroopa::KoopaTroopa(DOUBLE2& startingPos, Level* levelPtr, COLOUR colour) :
	Enemy(TYPE::KOOPA_TROOPA, startingPos, GetWidth(), GetHeight(), SpriteSheetManager::koopaTroopaPtr, BodyType::DYNAMIC, levelPtr, this),
	m_Color(colour)
{
	m_DirFacing = FacingDirection::RIGHT;
	m_AnimInfo.secondsPerFrame = 0.14;
	m_AnimationState = ANIMATION_STATE::WALKING;
}

KoopaTroopa::~KoopaTroopa()
{
}

void KoopaTroopa::Tick(double deltaTime)
{
	if (m_ShouldBeRemoved)
	{
		m_LevelPtr->RemoveEnemy(this);
		return;
	}

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
		m_ActPtr->AddBoxFixture(GetWidth(), GetHeight(), 0.0);

		double newHalfHeight = GetHeight() / 2;

		double newCenterY = m_ActPtr->GetPosition().y + (newHalfHeight - oldHalfHeight);
		m_ActPtr->SetPosition(DOUBLE2(m_ActPtr->GetPosition().x, newCenterY));

		m_NeedsNewFixture = false;
	}

	if (m_AnimationState == ANIMATION_STATE::UPSIDEDOWN_SHELLESS)
	{
		if (m_ActPtr->GetPosition().y > Game::HEIGHT + GetHeight())
		{
			m_ShouldBeRemoved = true;
		}

		return;
	}

	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 2;

	if (m_FramesSpentTurningAround > -1)
	{
		if (++m_FramesSpentTurningAround > FRAMES_OF_TURNAROUND)
		{
			m_FramesSpentTurningAround = -1;
		}
	}

	//DOUBLE2 koopaFeet = m_ActPtr->GetPosition() + DOUBLE2(0, GetHeight());


	double xVel = WALK_VEL;
	if (m_DirFacing == FacingDirection::LEFT)
	{
		xVel = -xVel;
	}

	m_ActPtr->SetLinearVelocity(DOUBLE2(xVel, m_ActPtr->GetLinearVelocity().y));
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
		centerY += 10;
	}

	if (xScale != 1 || yScale != 1)
	{
		MATRIX3X2 matReflect = MATRIX3X2::CreateScalingMatrix(DOUBLE2(xScale, yScale));
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(centerX, centerY);
		GAME_ENGINE->SetWorldMatrix(matTranslate.Inverse() * matReflect * matTranslate * matPrevWorld);
	}

	DOUBLE2 animationFrame = DetermineAnimationFrame();
	m_SpriteSheetPtr->Paint(centerX, centerY + 2, animationFrame.x, animationFrame.y);

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
		OutputDebugString(String("Unhandled colour in KoopaTroopa::DetermineAnimationFrame\n"));
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

void KoopaTroopa::Hit()
{
	switch (m_AnimationState)
	{
	case ANIMATION_STATE::WALKING:
	{
		ChangeAnimationState(ANIMATION_STATE::SHELLESS);
	} break;
	case ANIMATION_STATE::SHELLESS:
	{
		ChangeAnimationState(ANIMATION_STATE::UPSIDEDOWN_SHELLESS);
		Die();
	} break;
	case ANIMATION_STATE::WALKING_SHELLESS:
	{
		ChangeAnimationState(ANIMATION_STATE::SQUASHED);
		Die();
	} break;
	}
}

void KoopaTroopa::ChangeAnimationState(ANIMATION_STATE newAnimationState)
{

	m_AnimationState = newAnimationState;

	m_NeedsNewFixture = true;

	// LATER: Play sound here?
	// LATER: Add particle?
}

void KoopaTroopa::Die()
{
	if (m_AnimationState == ANIMATION_STATE::UPSIDEDOWN_SHELLESS)
	{
		// NOTE: We're already dying!
		OutputDebugString(String("Koopa Troopa was told to die more than once!\n"));
		return;
	}
	// LATER: Spawn poof particle
	
	m_ActPtr->SetSensor(true);
	m_ActPtr->SetLinearVelocity(DOUBLE2(0, -200));
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
	static const int SMALL_HEIGHT = 12;
	static const int LARGE_HEIGHT = 26;

	if (m_AnimationState == ANIMATION_STATE::WALKING)
	{
		return LARGE_HEIGHT;
	}
	else
	{
		return SMALL_HEIGHT;
	}
}