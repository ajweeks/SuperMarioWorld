#include "stdafx.h"
#include "Camera.h"
#include "Player.h"
#include "Level.h"

#define GAME_ENGINE (GameEngine::GetSingleton())

Camera::Camera(int width, int height, Level* levelPtr) : m_Width(width), m_Height(height)
{
	Reset();
}

Camera::~Camera()
{
}

// TODO: Add yo functionality for certain levels
MATRIX3X2 Camera::GetViewMatrix(Player* playerPtr, Level* levelPtr)
{
	DOUBLE2 newTranslation = m_PrevTranslation;

	DOUBLE2 playerPos = playerPtr->GetPosition();

	if (playerPtr->GetDirectionFacing() == FACING_DIRECTION::LEFT)
	{
		// TODO:This hacky af but it works, change it later perhaps
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
	else // The player is facing right
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
	double panSpeed = 8;
	double difference = abs(m_PrevTranslation.x - m_TargetOffset.x);
	if (difference < panSpeed)
	{
		panSpeed = difference;
	}
	if (difference > epsilon)
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

	// TODO: Adjust camera's y differently?
	// newTranslation.y = playerPos.y - GAME_ENGINE->GetHeight() / 2;

	Clamp(newTranslation, levelPtr);

	m_PrevTranslation = newTranslation;
	
	// NOTE: Negative position instead of .Inverse()
	MATRIX3X2 matCameraTranslation = MATRIX3X2::CreateTranslationMatrix(-newTranslation);

	return matCameraTranslation;
}

/* Paints extra debug info about the camera (Expects view matrix to be identity) */
void Camera::DEBUGPaint()
{
	//GAME_ENGINE->SetColor(COLOR(250, 20, 20));
	//GAME_ENGINE->DrawLine(m_XOffset, 0, m_XOffset, GAME_ENGINE->GetHeight());

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

void Camera::Reset()
{
	m_PrevTranslation = DOUBLE2(0.0, m_Height);
	m_TargetOffset = DOUBLE2(0.0, 0.0);
	m_XOffset = 0;
}