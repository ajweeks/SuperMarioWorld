#include "stdafx.h"

#include "GoalGate.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "SoundManager.h"

const double GoalGate::BAR_SPEED = 1.05;

GoalGate::GoalGate(DOUBLE2 topLeft, Level* levelPtr) :
	Gate(topLeft + DOUBLE2(9, 16), levelPtr, TYPE::GOAL_GATE, BAR_LENGTH, BAR_DIAMETER)
{
	m_AnimInfo.secondsPerFrame = 0.1;

	m_TopLeft = topLeft;

	m_ActPtr->SetSensor(true);
}

GoalGate::~GoalGate()
{
}

void GoalGate::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 4;

	if (m_IsHit == false)
	{
		m_BarHeight += m_BarDirectionMoving * BAR_SPEED;

		int maxBarHeight = (TILES_HIGH - 1) * TILE_SIZE;
		if (m_BarHeight <= 0) 
		{
			m_BarDirectionMoving = FacingDirection::OppositeDirection(m_BarDirectionMoving);
			m_BarHeight = 0;
		}
		else if (m_BarHeight >= maxBarHeight)
		{
			m_BarDirectionMoving = FacingDirection::OppositeDirection(m_BarDirectionMoving);
			m_BarHeight = maxBarHeight;
		}
	}

	m_ActPtr->SetPosition(DOUBLE2(m_ActPtr->GetPosition().x, m_TopLeft.y + TILES_HIGH * TILE_SIZE - m_BarHeight - BAR_DIAMETER/2));
}

void GoalGate::Paint()
{
	double left = m_TopLeft.x;
	double top = m_TopLeft.y;
	
	double x = left + TILE_SIZE / 2;
	double y = top + TILE_SIZE / 2;

	int srcX = 4;
	int srcY = 7;

	// TOPS OF POLES
	SpriteSheetManager::generalTilesPtr->Paint(x, y, srcX, srcY);
	SpriteSheetManager::generalTilesPtr->Paint(x + TILE_SIZE * 2, y, srcX + 1, srcY);

	srcY++;
	y += TILE_SIZE;

	// LEFT POLE
	for (int i = 0; i < TILES_HIGH - 1; ++i)
	{
		SpriteSheetManager::generalTilesPtr->Paint(x, y, srcX, srcY);
		y += TILE_SIZE;
	}

	// BAR
	if (m_IsHit == false)
	{
		x = m_ActPtr->GetPosition().x - BAR_LENGTH / 2;
		y = m_ActPtr->GetPosition().y - BAR_DIAMETER / 2;
		RECT2 srcRect(64, 389, 87, 397);
		GAME_ENGINE->DrawBitmap(SpriteSheetManager::generalTilesPtr->GetBitmap(), DOUBLE2(x, y), srcRect);
	}
}

void GoalGate::PaintFrontPole()
{
	double left = m_TopLeft.x;
	double top = m_TopLeft.y;

	double x = left + 5 * TILE_SIZE / 2.0;
	double y = top + 3 * TILE_SIZE / 2.0;

	int srcX = 5;
	int srcY = 8;

	// RIGHT POLE
	for (int i = 0; i < TILES_HIGH - 1; ++i)
	{
		SpriteSheetManager::generalTilesPtr->Paint(x, y, srcX, srcY);
		y += TILE_SIZE;
	}
}

void GoalGate::Hit()
{
	m_IsHit = true;

	// LATER: Start level end sounds
	//SoundManager::PlaySoundEffect(SoundManager::SOUND::MIDWAY_GATE_PASSTHROUGH);
}

bool GoalGate::IsHit()
{
	return m_IsHit;
}
