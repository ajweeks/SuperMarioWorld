#include "stdafx.h"

#include "Yoshi.h"
#include "Level.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "Player.h"

Yoshi::Yoshi(DOUBLE2 position, Level* levelPtr) : 
	Entity(position, BodyType::DYNAMIC, levelPtr, ActorId::YOSHI, this)
{
	m_ActPtr->AddBoxFixture(GetWidth(), GetHeight(), 0.0);
	m_ActPtr->SetFixedRotation(true);

	m_AnimInfo.secondsPerFrame = 0.9;
	m_AnimationState = ANIMATION_STATE::EGG;

	m_SpriteSheetPtr = SpriteSheetManager::smallYoshiPtr;

	m_HatchingTimer = CountdownTimer(140);
	m_HatchingTimer.Start();
}

Yoshi::~Yoshi()
{
}

void Yoshi::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 2;

	if (m_HatchingTimer.Tick() && m_HatchingTimer.IsComplete())
	{
		ChangeAnimationState(ANIMATION_STATE::WAITING);
	}

	if (m_IsCarryingMario)
	{
		m_ActPtr->SetPosition(m_PlayerPtr->GetPosition());
	}
	else if (m_IsRunning)
	{

	}
}

void Yoshi::Paint()
{
	// NOTE: When the player is riding yoshi they paint both characters (since they are both in one spritesheet)
	if (m_IsCarryingMario) return;

	double centerX = m_ActPtr->GetPosition().x;
	double centerY = m_ActPtr->GetPosition().y;
	PaintAnimationFrame(centerX - WIDTH/2, centerY - HEIGHT/2);
}

void Yoshi::PaintAnimationFrame(double left, double top)
{
	int srcX = 0, srcY = 0;

	switch (m_AnimationState)
	{
		// Small yoshi spritesheet
	case ANIMATION_STATE::EGG:
	{
		srcX = 0 + m_AnimInfo.frameNumber;
		srcY = 1;
	} break;
	case ANIMATION_STATE::BABY:
	{
		srcX = 0 + m_AnimInfo.frameNumber;
		srcY = 0;
	} break;

	// Big yoshi spritesheet
	case ANIMATION_STATE::WAITING:
	{
		srcX = 2 + m_AnimInfo.frameNumber;
		srcY = 0;
	} break;
	case ANIMATION_STATE::WALKING:
	{
		srcX = 0 + m_AnimInfo.frameNumber;
		srcY = 0;
	} break;
	}

	m_SpriteSheetPtr->Paint(left, top - 5, srcX, srcY);
}

void Yoshi::ChangeAnimationState(ANIMATION_STATE newAnimationState)
{
	m_AnimationState = newAnimationState;

	switch (newAnimationState)
	{
	case ANIMATION_STATE::WAITING:
	{
		m_SpriteSheetPtr = SpriteSheetManager::yoshiPtr;
	} break;
	}
}

void Yoshi::SetCarryingMario(bool carryingMario, Player* marioPtr)
{
	m_IsCarryingMario = carryingMario;
	m_PlayerPtr = marioPtr;
}

int Yoshi::GetWidth()
{
	return WIDTH;
}

int Yoshi::GetHeight()
{
	return HEIGHT;
}

bool Yoshi::IsHatching()
{
	return m_AnimationState == ANIMATION_STATE::EGG || m_AnimationState == ANIMATION_STATE::BABY;
}
