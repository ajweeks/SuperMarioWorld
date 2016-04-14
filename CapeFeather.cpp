#include "stdafx.h"

#include "CapeFeather.h"
#include "SpriteSheet.h"

CapeFeather::CapeFeather(DOUBLE2 topLeft, Level* levelPtr) :
	Item(topLeft, TYPE::CAPE_FEATHER, levelPtr)
{
}

void CapeFeather::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 2;
}

void CapeFeather::Paint()
{
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	double srcCol = 3;
	double srcRow = 8;

	m_SpriteSheetPtr->Paint(left, top, srcCol, srcRow);
}
