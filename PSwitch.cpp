#include "stdafx.h"

#include "PSwitch.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "SoundManager.h"
#include "Level.h"
#include "EnemyDeathCloudParticle.h"

PSwitch::PSwitch(DOUBLE2 topLeft, Colour colour, Level* levelPtr) :
	Item(topLeft, Item::Type::P_SWITCH, levelPtr, Level::ITEM, BodyType::DYNAMIC), m_Colour(colour)
{
	m_PressedTimer = CountdownTimer(20);
}

void PSwitch::Tick(double deltaTime)
{
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
	int srcRow = 11;
	if (m_IsPressed)
	{
		srcCol += 1;
	}
	SpriteSheetManager::generalTilesPtr->Paint(left, top + 2, srcCol, srcRow);
}

void PSwitch::Hit()
{
	m_IsPressed = true;
	m_PressedTimer.Start();
	
	SoundManager::PlaySoundEffect(SoundManager::Sound::PSWITCH_ACTIVATE);

	// TODO: Add screenshake here!
	
}