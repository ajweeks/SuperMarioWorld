#include "stdafx.h"

#include "KoopaShell.h"
#include "Game.h"
#include "SpriteSheet.h"
#include "SpriteSheetManager.h"
#include "SplatParticle.h"
#include "Level.h"
#include "Player.h"
#include "EnemyPoofParticle.h"
#include "SoundManager.h"

const double KoopaShell::HORIZONTAL_KICK_BASE_VEL = 190;
const double KoopaShell::VERTICAL_KICK_VEL = -520.0;
const double KoopaShell::SHELL_HIT_VEL = -180.0;

KoopaShell::KoopaShell(DOUBLE2 topLeft, Level* levelPtr, COLOUR colour, bool upsideDown)  :
	Item(topLeft, TYPE::KOOPA_SHELL, levelPtr, Level::SHELL, BodyType::DYNAMIC, WIDTH, HEIGHT),
	m_Colour(colour)
{
	b2Filter collisionFilter = m_ActPtr->GetCollisionFilter();
	collisionFilter.maskBits |= Level::ENEMY | Level::SHELL;
	m_ActPtr->SetCollisionFilter(collisionFilter);

	if (upsideDown)
	{
		ShellHit();
	}
}

KoopaShell::~KoopaShell()
{
}

void KoopaShell::Tick(double deltaTime)
{
	if (m_ShouldBeRemoved)
	{
		m_LevelPtr->RemoveItem(this);
		return;
	}

	if (m_IsFalling)
	{
		if (m_ActPtr->GetPosition().y > m_LevelPtr->GetHeight() + HEIGHT)
		{
			m_LevelPtr->RemoveItem(this);
		}
		return;
	}

	if (m_IsBouncing)
	{
		if (m_ActPtr->GetLinearVelocity().y == 0.0)
		{
			m_IsBouncing = false;
		}
	}

	if (m_IsMoving)
	{
		double horVel = HORIZONTAL_KICK_BASE_VEL;
		m_ActPtr->SetLinearVelocity(DOUBLE2(horVel * m_DirMoving, m_ActPtr->GetLinearVelocity().y));

		m_AnimInfo.Tick(deltaTime);
		m_AnimInfo.frameNumber %= 3;

		DOUBLE2 point1 = m_ActPtr->GetPosition();
		DOUBLE2 point2 = m_ActPtr->GetPosition() + DOUBLE2(m_DirMoving * (WIDTH / 2 + 2), 0);
		DOUBLE2 intersection, normal;
		double fraction = -1.0;
		int collisionBits = Level::LEVEL | Level::BLOCK;

		if (m_LevelPtr->Raycast(point1, point2, collisionBits, intersection, normal, fraction))
		{
			m_DirMoving = -m_DirMoving;
		}

		// Prevent moving off the left side of the level
		if (m_ActPtr->GetPosition().x < -WIDTH)
		{
			m_LevelPtr->RemoveItem(this);
		}
	}
}

void KoopaShell::Paint()
{
	int srcCol = 0;
	if (m_Colour == COLOUR::RED) srcCol = 1;

	int srcRow = 0 + m_AnimInfo.frameNumber;

	double centerX = m_ActPtr->GetPosition().x;
	double centerY = m_ActPtr->GetPosition().y;

	MATRIX3X2 matPrevWorld = GAME_ENGINE->GetWorldMatrix();
	if (m_IsFalling)
	{
		srcRow = 0;

		MATRIX3X2 matReflect = MATRIX3X2::CreateScalingMatrix(DOUBLE2(1, -1));
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(centerX, centerY);
		MATRIX3X2 matTranslateInverse = MATRIX3X2::CreateTranslationMatrix(-centerX, -centerY);
		GAME_ENGINE->SetWorldMatrix(matTranslateInverse * matReflect * matTranslate * matPrevWorld);
	}

	SpriteSheetManager::koopaShellPtr->Paint(centerX, centerY + 2.5, srcCol, srcRow);

	GAME_ENGINE->SetWorldMatrix(matPrevWorld);
}

void KoopaShell::KickHorizontally(int facingDir, bool wasThrown)
{
	SoundManager::PlaySoundEffect(SoundManager::SOUND::SHELL_KICK);

	m_IsMoving = true;
	m_DirMoving = facingDir;

	if (wasThrown)
	{
		SplatParticle* splatParticlePtr = new SplatParticle(m_ActPtr->GetPosition() + DOUBLE2(m_DirMoving * -3, 0));
		m_LevelPtr->AddParticle(splatParticlePtr);
	}
}

void KoopaShell::KickVertically(double horizontalVel)
{
	SoundManager::PlaySoundEffect(SoundManager::SOUND::SHELL_KICK);

	SplatParticle* splatParticlePtr = new SplatParticle(m_ActPtr->GetPosition() + DOUBLE2(m_DirMoving * -3, 0));
	m_LevelPtr->AddParticle(splatParticlePtr);

	m_ActPtr->SetLinearVelocity(DOUBLE2(horizontalVel, VERTICAL_KICK_VEL));
	m_IsBouncing = true;
}

void KoopaShell::ShellHit()
{
	m_ActPtr->SetLinearVelocity(DOUBLE2(m_ActPtr->GetLinearVelocity().x, SHELL_HIT_VEL));
	m_ActPtr->SetSensor(true);

	m_IsFalling = true;
}

void KoopaShell::Stomp()
{
	EnemyPoofParticle* poofParticlePtr = new EnemyPoofParticle(m_ActPtr->GetPosition());
	m_LevelPtr->AddParticle(poofParticlePtr);

	m_LevelPtr->GetPlayer()->AddScore(200, m_ActPtr->GetPosition());

	SoundManager::PlaySoundEffect(SoundManager::SOUND::ENEMY_HEAD_STOMP_START);
	m_LevelPtr->GetPlayer()->ResetNumberOfFramesUntilEndStompSound();

	m_ShouldBeRemoved = true;
}

bool KoopaShell::IsFalling()
{
	return m_IsFalling;
}

bool KoopaShell::IsMoving()
{
	return m_IsMoving;
}

void KoopaShell::SetMoving(bool moving)
{
	m_IsMoving = moving;

	if (!moving)
	{
		m_ActPtr->SetLinearVelocity(DOUBLE2(0.0, 0.0));
	}
}


COLOUR KoopaShell::GetColour()
{
	return m_Colour;
}

bool KoopaShell::IsBouncing()
{
	return m_IsBouncing;
}
