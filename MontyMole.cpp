#include "stdafx.h"

#include "MontyMole.h"
#include "SpriteSheet.h"
#include "SpriteSheetManager.h"

MontyMole::MontyMole(DOUBLE2& startingPos, Level* levelPtr) : 
	Enemy(TYPE::MONTY_MOLE, startingPos, GetWidth(), GetHeight(), SpriteSheetManager::montyMolePtr, BodyType::DYNAMIC, levelPtr, this)
{
}

MontyMole::~MontyMole()
{
}

void MontyMole::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 2;
}

void MontyMole::Paint()
{
	DOUBLE2 playerPos = m_ActPtr->GetPosition();
	m_SpriteSheetPtr->Paint(playerPos.x, playerPos.y, m_AnimInfo.frameNumber, 0);
}

int MontyMole::GetWidth()
{
	return WIDTH;
}

int MontyMole::GetHeight()
{
	return HEIGHT;
}