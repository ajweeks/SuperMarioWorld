#include "stdafx.h"

#include "KoopaShell.h"
#include "Game.h"
#include "SpriteSheet.h"
#include "SpriteSheetManager.h"
#include "SplatParticle.h"
#include "Level.h"
#include "Player.h"
#include "EnemyPoofParticle.h"
#include "NumberParticle.h"
#include "SoundManager.h"

const double KoopaShell::HORIZONTAL_KICK_BASE_VEL = 190;
const double KoopaShell::VERTICAL_KICK_VEL = -520.0;
const double KoopaShell::SHELL_HIT_VEL = -180.0;

KoopaShell::KoopaShell(DOUBLE2 topLeft, Level* levelPtr, COLOUR colour, bool upsideDown)  :
	Item(topLeft, TYPE::KOOPA_SHELL, levelPtr, BodyType::DYNAMIC, WIDTH, HEIGHT),
	m_Colour(colour)
{
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
		int collisionBits = Level::COLLIDE_WITH_LEVEL | Level::COLLIDE_WITH_BLOCKS;

		if (m_LevelPtr->Raycast(point1, point2, collisionBits, intersection, normal, fraction))
		{
			m_DirMoving = -m_DirMoving;
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

	NumberParticle* numberParticlePtr = new NumberParticle(200, m_ActPtr->GetPosition());
	m_LevelPtr->AddParticle(numberParticlePtr);

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
		m_ActPtr->SetLinearVelocity(DOUBLE2());
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
