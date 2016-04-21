#include "stdafx.h"

#include "KoopaShell.h"
#include "SpriteSheet.h"
#include "SpriteSheetManager.h"
#include "SplatParticle.h"
#include "Level.h"
#include "Player.h"
#include "EnemyPoofParticle.h"
#include "NumberParticle.h"
#include "SoundManager.h"

const double KoopaShell::KICK_VEL = 320.0;
const double KoopaShell::SHELL_HIT_VEL = -250.0;

KoopaShell::KoopaShell(DOUBLE2 topLeft, Level* levelPtr, COLOUR colour)  :
	Item(topLeft, TYPE::KOOPA_SHELL, levelPtr, BodyType::DYNAMIC),
	m_Colour(colour)
{
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

	if (m_IsMoving)
	{
		m_ActPtr->SetLinearVelocity(DOUBLE2(KICK_VEL * m_DirMoving, 0));
		m_AnimInfo.Tick(deltaTime);
		m_AnimInfo.frameNumber %= 3;
	}
}

void KoopaShell::Paint()
{
	int srcCol = 0;
	if (m_Colour == COLOUR::RED) srcCol = 1;

	int srcRow = 0 + m_AnimInfo.frameNumber;

	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	SpriteSheetManager::koopaShellPtr->Paint(left, top, srcCol, srcRow);
}

void KoopaShell::Kick(int facingDir, bool wasThrown)
{
	m_IsMoving = true;
	m_DirMoving = facingDir;

	if (wasThrown)
	{
		SplatParticle* splatParticlePtr = new SplatParticle(m_ActPtr->GetPosition());
		m_LevelPtr->AddParticle(splatParticlePtr);
	}
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

bool KoopaShell::IsMoving()
{
	return m_IsMoving;
}