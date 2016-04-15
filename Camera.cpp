#include "stdafx.h"

#include "Game.h"
#include "Camera.h"
#include "Player.h"
#include "Level.h"

Camera::Camera(int width, int height, Level* levelPtr) : m_Width(width), m_Height(height)
{
	Reset();
}

Camera::~Camera()
{
}

void Camera::Reset()
{
	m_PrevTranslation = DOUBLE2(0.0, m_Height);
	m_TargetOffset = DOUBLE2(0.0, 0.0);
	m_XOffset = 0;
}

DOUBLE2 Camera::GetOffset(Player* playerPtr, Level* levelPtr)
{
#ifdef SMW_ENABLE_JUMP_TO
	if (GAME_ENGINE->IsKeyboardKeyPressed('O'))
	{
		m_PrevTranslation.x = SMW_JUMP_TO_POS_X - Game::WIDTH / 2;
	}
#endif

	DOUBLE2 newTranslation = m_PrevTranslation;

	DOUBLE2 playerPos = playerPtr->GetPosition();

	if (playerPtr->GetDirectionFacing() == FacingDirection::LEFT)
	{
		if (m_XOffset == MAX_OFFSET)
		{
			m_TargetOffset.x = playerPos.x - MAX_OFFSET;
		}
		else if (playerPos.x - m_PrevTranslation.x < m_XOffset - MAX_BACKTRACK_DISTANCE)
		{
			m_TargetOffset.x = playerPos.x - MAX_OFFSET;
			m_XOffset = MAX_OFFSET;
		}
	}
	else if (playerPtr->GetDirectionFacing() == FacingDirection::RIGHT)
	{
		if (m_XOffset == -(MAX_OFFSET - m_Width))
		{
			m_TargetOffset.x = playerPos.x + MAX_OFFSET - m_Width;
		}
		else if (playerPos.x - m_PrevTranslation.x >= m_XOffset + MAX_BACKTRACK_DISTANCE)
		{
			m_TargetOffset.x = playerPos.x + MAX_OFFSET - m_Width;
			m_XOffset = -(MAX_OFFSET - m_Width);
		}
	}

	double epsilon = 0.001;
	double panSpeed = 0.25;
	double difference = abs(m_TargetOffset.x - m_PrevTranslation.x);

	// We will overshoot if we use the whole panSpeed, just use the difference
	if (difference < panSpeed)
	{
		panSpeed = difference;
	}
	if (panSpeed > epsilon)
	{
		if (m_TargetOffset.x > m_PrevTranslation.x)
		{
			newTranslation.x = m_PrevTranslation.x + panSpeed;
		}
		else
		{
			newTranslation.x = m_PrevTranslation.x - panSpeed;
		}
	}

	// TODO: Adjust camera's y
	// newTranslation.y = playerPos.y - Game::HEIGHT / 2;

	newTranslation.y = 550;
	Clamp(newTranslation, levelPtr);

	m_PrevTranslation = newTranslation;

	return newTranslation;
}

// TODO: Add yo functionality for certain levels
MATRIX3X2 Camera::GetViewMatrix(Player* playerPtr, Level* levelPtr)
{
	DOUBLE2 newTranslation = GetOffset(playerPtr, levelPtr);
	
	// NOTE: Uses negative position instead of .Inverse()
	MATRIX3X2 matCameraTranslation = MATRIX3X2::CreateTranslationMatrix(-newTranslation);

	return matCameraTranslation;
}

/* Paints extra debug info about the camera (Expects view matrix to be Game::matIdentity) */
void Camera::DEBUGPaint()
{
	double centerX = m_Width / 2;
	double x = 0;

	GAME_ENGINE->SetColor(COLOR(20, 20, 250));
	x = MAX_OFFSET + MAX_BACKTRACK_DISTANCE;
	GAME_ENGINE->DrawLine(x, 0, x, m_Height);
	x = m_Width - MAX_OFFSET - MAX_BACKTRACK_DISTANCE;
	GAME_ENGINE->DrawLine(x, 0, x, m_Height);

	GAME_ENGINE->SetColor(COLOR(20, 200, 20));
	x = MAX_OFFSET;
	GAME_ENGINE->DrawLine(x, 0, x, m_Height);
	x = m_Width - MAX_OFFSET;
	GAME_ENGINE->DrawLine(x, 0, x, m_Height);
}

void Camera::Clamp(DOUBLE2& posRef, Level* levelPtr)
{
	if (posRef.x < 0)
	{
		posRef.x = 0;
	}
	else if (posRef.x > levelPtr->GetWidth() - m_Width)
	{
		posRef.x = levelPtr->GetWidth() - m_Width;
	}

	if (posRef.y < 0)
	{
		posRef.y = 0;
	}
	else if (posRef.y > levelPtr->GetHeight() - m_Height)
	{
		posRef.y = levelPtr->GetHeight() - m_Height;
	}
}
