#include "stdafx.h"

#include "PSwitch.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "SoundManager.h"
#include "Level.h"
#include "EnemyDeathCloudParticle.h"

PSwitch::PSwitch(DOUBLE2 topLeft, Level* levelPtr) :
	Item(topLeft + DOUBLE2(8 - WIDTH / 2, 0), Item::Type::P_SWITCH, levelPtr, Level::ITEM, BodyType::DYNAMIC, WIDTH, HEIGHT)
{
	m_PressedTimer = SMWTimer(20);
}

void PSwitch::Tick(double deltaTime)
{
	Item::Tick(deltaTime);

	m_ActPtr->SetAwake(true); // This line ensures that this p switch will fall through rotating blocks reliably

	if (m_PressedTimer.Tick() && m_PressedTimer.IsComplete())
	{
		EnemyDeathCloudParticle* cloudParticlePtr = new EnemyDeathCloudParticle(m_ActPtr->GetPosition());
		m_LevelPtr->AddParticle(cloudParticlePtr);

		m_LevelPtr->RemoveItem(this);
	}
}

void PSwitch::Paint()
{
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	int srcCol = 4;
	int srcRow = 13;
	if (m_IsPressed)
	{
		srcCol += 1;
	}
	SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES)->Paint(left, top + 2, srcCol, srcRow);
}

void PSwitch::Hit()
{
	m_IsPressed = true;
	m_PressedTimer.Start();
	
	SoundManager::PlaySoundEffect(SoundManager::Sound::PSWITCH_ACTIVATE);

	// TODO: Add screenshake here!
}
