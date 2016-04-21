#include "stdafx.h"

#include "PSwitch.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

PSwitch::PSwitch(DOUBLE2 topLeft, COLOUR colour, Level* levelPtr) :
	Item(topLeft, Item::TYPE::P_SWITCH, levelPtr, BodyType::DYNAMIC), m_Colour(colour)
{
}

void PSwitch::Tick(double deltaTime)
{
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
	SpriteSheetManager::generalTilesPtr->Paint(left, top, srcCol, srcRow);
}
