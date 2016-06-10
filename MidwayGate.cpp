#include "stdafx.h"

#include "MidwayGate.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "SoundManager.h"

MidwayGate::MidwayGate(DOUBLE2 topLeft, Level* levelPtr, int barHeight) :
	Gate(topLeft + DOUBLE2(5, TILES_HIGH * TILE_SIZE - barHeight), levelPtr, Type::MIDWAY_GATE, BAR_LENGTH, BAR_DIAMETER)
{
	m_AnimInfo.secondsPerFrame = 0.14;

	m_TopLeft = topLeft; 
	m_BarHeight =barHeight;

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

	const SpriteSheet* generalTilesPtr = SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES);

	// LEFT POLE
	double srcX = m_AnimInfo.frameNumber * POLE_WIDTH * 2;
	srcRect = RECT2(srcX, 272, srcX + POLE_WIDTH, 272 + 64);
	GAME_ENGINE->DrawBitmap(generalTilesPtr->GetBitmap(), DOUBLE2(x, y), srcRect);


	// BAR
	if (m_IsHit) srcRect = RECT2(64, 333, 64 + 3, 333 + 3);
	else srcRect = RECT2(64, 333, 64 + 19, 333 + 3);

	x = m_ActPtr->GetPosition().x - BAR_LENGTH / 2.0;
	y = m_ActPtr->GetPosition().y - BAR_DIAMETER / 2.0 - 1;
	GAME_ENGINE->DrawBitmap(generalTilesPtr->GetBitmap(), DOUBLE2(x, y), srcRect);
}

void MidwayGate::PaintFrontPole()
{
	double left = m_TopLeft.x;
	double top = m_TopLeft.y;

	double x = left + TILES_WIDE * TILE_SIZE - POLE_WIDTH;
	double y = top;

	// RIGHT POLE
	double srcX = m_AnimInfo.frameNumber * POLE_WIDTH * 2 + POLE_WIDTH;
	RECT2 srcRect = RECT2(srcX, 272, srcX + POLE_WIDTH, 272 + 64);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES)->GetBitmap(), DOUBLE2(x, y), srcRect);
}

void MidwayGate::Hit()
{
	m_IsHit = true;

	SoundManager::PlaySoundEffect(SoundManager::Sound::MIDWAY_GATE_PASSTHROUGH);
}

bool MidwayGate::IsHit()
{
	return m_IsHit;
}
