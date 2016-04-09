#include "stdafx.h"

#include "KoopaTroopa.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

KoopaTroopa::KoopaTroopa(DOUBLE2& startingPos, Level* levelPtr) :
	Enemy(TYPE::KOOPA_TROOPA, startingPos, GetWidth(), GetHeight(), SpriteSheetManager::koopaTroopaPtr, BodyType::DYNAMIC, levelPtr, this)
{
	m_DirFacing = FacingDirection::RIGHT;
}

KoopaTroopa::~KoopaTroopa()
{
}

void KoopaTroopa::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 2;
}

void KoopaTroopa::Paint()
{
	DOUBLE2 playerPos = m_ActPtr->GetPosition();
	m_SpriteSheetPtr->Paint(playerPos.x, playerPos.y, m_AnimInfo.frameNumber, 0);
}

int KoopaTroopa::GetWidth()
{
	return WIDTH;
}

int KoopaTroopa::GetHeight()
{
	return HEIGHT;
}