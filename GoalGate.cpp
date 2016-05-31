#include "stdafx.h"

#include "GoalGate.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "SoundManager.h"
#include "Level.h"
#include "Player.h"
#include "Coin.h"

const double GoalGate::BAR_SPEED = 1.05;

GoalGate::GoalGate(DOUBLE2 topLeft, Level* levelPtr) :
	Gate(topLeft + DOUBLE2(9, 16), levelPtr, Type::GOAL_GATE, BAR_LENGTH, BAR_DIAMETER)
{
	m_AnimInfo.secondsPerFrame = 0.1;

	m_TopLeft = topLeft;

	m_ActPtr->SetSensor(true);
	m_BarDirectionMoving = 1;
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
			m_BarDirectionMoving = -m_BarDirectionMoving;
			m_BarHeight = 0;
		}
		else if (m_BarHeight >= maxBarHeight)
		{
			m_BarDirectionMoving = -m_BarDirectionMoving;
			m_BarHeight = maxBarHeight;
		}

		// Mario can walk through the gate, or hit the bar
		// If he simply walks through it, the bar is turned into a coin
		if (m_LevelPtr->GetPlayer()->GetPosition().x > m_TopLeft.x + (TILES_WIDE / 2) * TILE_SIZE)
		{
			m_IsHit = true;
			m_LevelPtr->TriggerEndScreen();

			Coin* coinPtr = new Coin(m_TopLeft + DOUBLE2((TILES_WIDE/2) * TILE_SIZE, TILES_HIGH * TILE_SIZE - m_BarHeight), m_LevelPtr);
			m_LevelPtr->AddItem(coinPtr);

			// LATER: Add smoke poof particle
		}

		m_ActPtr->SetPosition(DOUBLE2(m_ActPtr->GetPosition().x, m_TopLeft.y + TILES_HIGH * TILE_SIZE - m_BarHeight - BAR_DIAMETER/2));
	}
}

void GoalGate::Paint()
{
	double left = m_TopLeft.x;
	double top = m_TopLeft.y;
	
	double x = left + TILE_SIZE / 2;
	double y = top + TILE_SIZE / 2;

	int srcX = 4;
	int srcY = 7;

	SpriteSheet* generalTilesPtr = SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES);

	// TOPS OF POLES
	generalTilesPtr->Paint(x, y, srcX, srcY);
	generalTilesPtr->Paint(x + TILE_SIZE * 2, y, srcX + 1, srcY);

	srcY++;
	y += TILE_SIZE;

	// LEFT POLE
	for (int i = 0; i < TILES_HIGH - 1; ++i)
	{
		generalTilesPtr->Paint(x, y, srcX, srcY);
		y += TILE_SIZE;
	}

	// BAR
	if (m_IsHit == false)
	{
		x = m_ActPtr->GetPosition().x - BAR_LENGTH / 2.0;
		y = m_ActPtr->GetPosition().y - BAR_DIAMETER / 2.0;
		RECT2 srcRect(64, 389, 87, 397);
		GAME_ENGINE->DrawBitmap(generalTilesPtr->GetBitmap(), DOUBLE2(x, y), srcRect);
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
		SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES)->Paint(x, y, srcX, srcY);
		y += TILE_SIZE;
	}
}

void GoalGate::Hit()
{
	m_IsHit = true;
	m_LevelPtr->TriggerEndScreen(int(m_BarHeight));
}

bool GoalGate::IsHit()
{
	return m_IsHit;
}
