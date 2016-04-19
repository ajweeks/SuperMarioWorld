#include "stdafx.h"

#include "Berry.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

Berry::Berry(DOUBLE2& positionRef, Level* levelPtr, COLOUR colour) :
	Item(positionRef, TYPE::BERRY, levelPtr),
	m_Colour(colour)
{
	m_ActPtr->SetSensor(true);
	m_AnimInfo.secondsPerFrame = 0.125;
}

Berry::~Berry()
{
}

void Berry::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 4;
}

void Berry::Paint()
{
	assert (m_Colour != COLOUR::NONE);

	double left = m_ActPtr->GetPosition().x;
	if (m_AnimInfo.frameNumber == 0) left -= 1;
	if (m_AnimInfo.frameNumber == 2) left += 1;
	
	double top = m_ActPtr->GetPosition().y;
	double srcCol = 11 + m_AnimInfo.frameNumber;
	if (m_AnimInfo.frameNumber == 3) 
	{
		srcCol -= 2;
	}

	double srcRow = 22;
	if (m_Colour == COLOUR::PINK) srcRow += 1;
	else if (m_Colour == COLOUR::GREEN) srcRow += 2;
	SpriteSheetManager::generalTilesPtr->Paint(left, top, srcCol, srcRow);
}
