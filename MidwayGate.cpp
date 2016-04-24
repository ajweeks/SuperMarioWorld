#include "stdafx.h"

#include "MidwayGate.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "SoundManager.h"

MidwayGate::MidwayGate(DOUBLE2 topLeft, Level* levelPtr, int barHeight) :
	Item(topLeft + DOUBLE2(5, TILES_HIGH * TILE_SIZE - barHeight), Item::TYPE::MIDWAY_GATE, levelPtr, 
		BodyType::STATIC, TILES_WIDE * TILE_SIZE - 12, BAR_HEGHT),
	m_TopLeft(topLeft), m_BarHeight(barHeight)
{
	m_AnimInfo.secondsPerFrame = 0.14;

	m_ActPtr->SetSensor(true);
}

MidwayGate::~MidwayGate()
{
}


void MidwayGate::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 4;
}

void MidwayGate::Paint()
{
	double left = m_TopLeft.x;
	double top = m_TopLeft.y;

	double x = left;
	double y = top;
	RECT2 srcRect;

	double srcX = m_AnimInfo.frameNumber * POLE_WIDTH * 2;
	srcRect = RECT2(srcX, 336, srcX + POLE_WIDTH, 400);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::generalTilesPtr->GetBitmap(), DOUBLE2(x, y), srcRect);

	srcX += POLE_WIDTH;
	x += TILES_WIDE * TILE_SIZE - POLE_WIDTH;
	srcRect = RECT2(srcX, 336,  srcX + POLE_WIDTH, 400);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::generalTilesPtr->GetBitmap(), DOUBLE2(x, y), srcRect);


	if (m_IsHit) srcRect = RECT2(64, 397, 67, 400);
	else srcRect = RECT2(64, 397, 83, 400);

	x = m_ActPtr->GetPosition().x - (TILES_WIDE * TILE_SIZE - 12)/2;
	y = m_ActPtr->GetPosition().y - BAR_HEGHT/2 - 1;
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::generalTilesPtr->GetBitmap(), DOUBLE2(x, y), srcRect);
}

void MidwayGate::Hit()
{
	m_IsHit = true;

	SoundManager::PlaySoundEffect(SoundManager::SOUND::MIDWAY_GATE_PASSTHROUGH);
}

bool MidwayGate::IsHit()
{
	return m_IsHit;
}
