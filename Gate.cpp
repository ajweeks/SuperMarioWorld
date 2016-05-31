#include "stdafx.h"

#include "Gate.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "SoundManager.h"

Gate::Gate(DOUBLE2 topLeft, Level* levelPtr, Type itemType, int barLength, int barDiameter) :
	Item(topLeft, itemType, levelPtr, Level::ITEM,
		BodyType::STATIC, barLength, barDiameter),
	m_TopLeft(topLeft)
{
	m_AnimInfo.secondsPerFrame = 0.1;

	m_ActPtr->SetSensor(true);
	m_IsActive = true;
}

Gate::~Gate()
{
}
