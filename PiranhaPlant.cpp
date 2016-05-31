#include "stdafx.h"

#include "PiranhaPlant.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "INT2.h"
#include "Item.h"
#include "Player.h"

const double PiranhaPlant::VERTICAL_VEL_FAST = 105;
const double PiranhaPlant::VERTICAL_VEL_SLOW = 30;

PiranhaPlant::PiranhaPlant(DOUBLE2 topLeft, Level* levelPtr) :
	Enemy(Type::PIRHANA_PLANT, topLeft, WIDTH, HEIGHT, BodyType::DYNAMIC, levelPtr, this),
	m_StartingPostion(topLeft)
{
	m_VerticalVel = VERTICAL_VEL_FAST;
	m_DirMoving = Direction::UP;
	m_ActPtr->SetSensor(true);
	m_ActPtr->SetGravityScale(0.0);

	m_PausedAtTopTimer = CountdownTimer(25);
}

PiranhaPlant::~PiranhaPlant()
{
}

void PiranhaPlant::Tick(double deltaTime)
{
	Enemy::Tick(deltaTime);
	if (m_IsActive == false) return;

	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 4;

	if (m_PausedAtBottom)
	{
		if (IsPlayerNearby())
		{
			m_ActPtr->SetAwake(true);
			return;
		}
		else
		{
			m_PausedAtBottom = false;

			m_DirMoving *= -1;
			m_VerticalVel = VERTICAL_VEL_FAST;
		}
	}

	DOUBLE2 prevPos = m_ActPtr->GetPosition();

	double currentHeight = m_StartingPostion.y - prevPos.y;

	if (m_PausedAtTopTimer.Tick())
	{
		if (m_PausedAtTopTimer.IsComplete())
		{
			m_DirMoving *= -1;
		}
		else
		{
			return;
		}
	}
	else if (m_DirMoving == Direction::UP)
	{
		if (m_VerticalVel <= VERTICAL_VEL_FAST &&
			m_VerticalVel > VERTICAL_VEL_SLOW &&
			currentHeight > SLOW_DOWN_HEIGHT * Item::TILE_SIZE)
		{
			m_VerticalVel -= VERTICAL_VEL_SLOW * 0.15;

			if (m_VerticalVel < VERTICAL_VEL_SLOW) m_VerticalVel = VERTICAL_VEL_SLOW;
		}
		else if (m_PausedAtTopTimer.IsActive() == false && currentHeight > MAX_TILE_HEIGHT * Item::TILE_SIZE)
		{
			m_PausedAtTopTimer.Start();
		}
	}
	else if (m_DirMoving == Direction::DOWN && currentHeight < 0)
	{
		if (IsPlayerNearby())
		{
			m_PausedAtBottom = true;
			return;
		}
		else
		{
			m_DirMoving *= -1;
			m_VerticalVel = VERTICAL_VEL_FAST;
		}
	}

	m_ActPtr->SetPosition(DOUBLE2(prevPos.x, prevPos.y + m_DirMoving * m_VerticalVel * deltaTime));
}

bool PiranhaPlant::IsPlayerNearby()
{
	DOUBLE2 playerPos = m_LevelPtr->GetPlayer()->GetPosition();
	int minimumDistance = 30;
	return !(abs(playerPos.x - m_ActPtr->GetPosition().x) > minimumDistance &&
		abs(playerPos.y - m_ActPtr->GetPosition().y) > minimumDistance);
}

void PiranhaPlant::Paint()
{
	MATRIX3X2 matPrevWorld = GAME_ENGINE->GetWorldMatrix();

	double centerX = m_ActPtr->GetPosition().x;
	double centerY = m_ActPtr->GetPosition().y;

	INT2 animationFrame = INT2(m_AnimInfo.frameNumber, 0);
	double yo = 0;
	SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::PIRANHA_PLANT)->Paint(centerX, centerY + yo, animationFrame.x, animationFrame.y);

	GAME_ENGINE->SetWorldMatrix(matPrevWorld);
}

int PiranhaPlant::GetWidth()
{
	return WIDTH;
}

int PiranhaPlant::GetHeight()
{
	return HEIGHT;
}
