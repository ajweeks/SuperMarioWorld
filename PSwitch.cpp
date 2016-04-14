#include "stdafx.h"

#include "PSwitch.h"
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
	double srcCol = 4;
	double srcRow = 11;
	if (m_IsPressed)
	{
		srcCol += 1;
	}
	m_SpriteSheetPtr->Paint(left, top, srcCol, srcRow);
}
