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

// TODO(AJ): Add cushion zone in center of screen
// TODO(AJ): Ensure all cases are correct
MATRIX3X2 Camera::GetViewMatrix(Player* playerPtr, Level* levelPtr)
{
	DOUBLE2 newTranslation = m_PrevTranslation;
	//TrackAvatar(pos, playerPtr);

	DOUBLE2 playerPos = playerPtr->GetPosition();

	switch (playerPtr->GetDirectionFacing())
	{
	case FACING_DIRECTION::LEFT:
	{
		if (playerPos.x - m_XOffset < m_PrevTranslation.x)
		{
			m_TargetOffset.x = playerPos.x - 350;
			m_XOffset = 350;
		}
	} break;
	case FACING_DIRECTION::RIGHT:
	{
		if (m_Width - (playerPos.x - m_PrevTranslation.x) < m_Width - m_XOffset)
		{
			m_TargetOffset.x = playerPos.x + 400 - m_Width;
			m_XOffset = -(400-m_Width);
		}
	} break;
	}

	double EPSILON = 0.01;
	double PAN_SPEED = 10;
	double difference = abs(m_PrevTranslation.x - m_TargetOffset.x);
	if (difference < PAN_SPEED)
	{
		PAN_SPEED = difference;
	}
	if (difference > EPSILON)
	{
		if (m_TargetOffset.x > m_PrevTranslation.x)
		{
			newTranslation.x = m_PrevTranslation.x + PAN_SPEED;
		}
		else
		{
			newTranslation.x = m_PrevTranslation.x - PAN_SPEED;
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

void Camera::TrackAvatar(DOUBLE2& posRef, Player* playerPtr)
{
	RECT2 playerRect = playerPtr->GetCameraRect();

	double playerWidth = playerRect.right - playerRect.left;
	double playerHeight = playerRect.bottom - playerRect.top;

	posRef.x = playerRect.left + playerWidth / 2 - m_Width / 2;
	posRef.y = playerRect.top + playerHeight / 2 - m_Height / 2;
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