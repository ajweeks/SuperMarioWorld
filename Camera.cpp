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
	m_PrevTranslation = DOUBLE2(0.0, 550);
	m_TargetOffset = DOUBLE2();
	m_XOffset = 0.0;
	m_UsingConstantYO = false;
}

DOUBLE2 Camera::GetOffset(DOUBLE2 playerPos, int directionFacing, Level* levelPtr)
{
#ifdef SMW_ENABLE_JUMP_TO
	if (GAME_ENGINE->IsKeyboardKeyPressed('O'))
	{
		m_PrevTranslation.x = SMW_JUMP_TO_POS_X - Game::WIDTH / 2;
		return m_PrevTranslation;
	}
#endif

	double xo = CalculateXTranslation(playerPos, directionFacing, levelPtr);
	double yo = CalculateYTranslation(playerPos, levelPtr);
	DOUBLE2 newTranslation = DOUBLE2(xo, yo);

	Clamp(newTranslation, levelPtr);

	m_PrevTranslation = newTranslation;

	return newTranslation;
}

double Camera::CalculateXTranslation(DOUBLE2 playerPos, int directionFacing, Level* levelPtr)
{
	double translationX = m_PrevTranslation.x;

	if (directionFacing == FacingDirection::LEFT)
	{
		if (m_XOffset == MAX_X_OFFSET)
		{
			m_TargetOffset.x = playerPos.x - MAX_X_OFFSET;
		}
		else if (playerPos.x - m_PrevTranslation.x < m_XOffset - HORIZONTAL_CUSHION_SIZE)
		{
			m_TargetOffset.x = playerPos.x - MAX_X_OFFSET;
			m_XOffset = MAX_X_OFFSET;
		}
	}
	else if (directionFacing == FacingDirection::RIGHT)
	{
		if (m_XOffset == -(MAX_X_OFFSET - m_Width))
		{
			m_TargetOffset.x = playerPos.x + MAX_X_OFFSET - m_Width;
		}
		else if (playerPos.x - m_PrevTranslation.x >= m_XOffset + HORIZONTAL_CUSHION_SIZE)
		{
			m_TargetOffset.x = playerPos.x + MAX_X_OFFSET - m_Width;
			m_XOffset = -(MAX_X_OFFSET - m_Width);
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
			translationX = m_PrevTranslation.x + panSpeed;
		}
		else
		{
			translationX = m_PrevTranslation.x - panSpeed;
		}
	}
	
	return translationX;
}

/*
This is how the camera pans vertically:

If the player hasn't climbed on a beanstalk, the y offset is constant.

If the player is currently climbing on a beanstalk, the camera follows the player,
similarly to the way it follows them horizontally (with a cusion area in the middle, except
vetically it doesn't give you an offset based on your current direction)

When the player gets off the beanstalk, the camera offset seems to be based on the last y value
that the player was on the ground for. This is overrided if the player passes below a certain height
relative to the screen. Once the player reaches ground level again, the camera y offset is set back to
it's original constant value again until the player climbs a beanstalk again.

bool m_ConstantYO = true;

if (player is climbing)
{
m_ConstantYO = false;
if (player.y - camera.yo > threshold1)
{
camera.targetY = player.y - threshold1;
}
else if (player.y - camera.yo < threshold2)
{
camera.targetY = player.y - threshold2;
}
}

if (m_ConstantYO == false)
{
if (player is on ground)
{
if (player.feetY is on ground level)
{
m_ConstantYO = true;
camera.targetY = constant value;
return;
}

camera.targetY = player.feetY - threshold;
}
else if (player.feetY - camera.yo > threshold)
{
camera.targetY = player.feetY - threshold;
}
}
*/

// TODO: The camera actually does pay attention to the player's y pos *if* they are running at full speed
double Camera::CalculateYTranslation(DOUBLE2 playerPos, Level* levelPtr)
{
	double translationY = m_PrevTranslation.y;
	Player* playerPtr = levelPtr->GetPlayer();
	double playerFeetY = playerPtr->GetPosition().y + playerPtr->GetHeight() / 2;

	if (playerPtr->GetAnimationState() == Player::ANIMATION_STATE::CLIMBING)
	{
		m_UsingConstantYO = false;
		if (playerPtr->GetPosition().y - m_PrevTranslation.y > BOTTOM_BOUNDARY)
		{
			m_TargetOffset.y = playerPtr->GetPosition().y - BOTTOM_BOUNDARY;
		}
		else if (playerPtr->GetPosition().y - m_PrevTranslation.y < TOP_BOUNDARY)
		{
			m_TargetOffset.y = playerPtr->GetPosition().y - TOP_BOUNDARY;
		}
	}

	if (m_UsingConstantYO == false)
	{
		if (playerPtr->IsOnGround())
		{
			if (playerFeetY > 335)
			{
				m_UsingConstantYO = true;
				m_TargetOffset.y = 550;
			}
			else
			{
				m_TargetOffset.y = playerFeetY - BOTTOM_BOUNDARY;
			}
		}
		else if (playerFeetY - m_PrevTranslation.y > BOTTOM_BOUNDARY)
		{
			m_TargetOffset.y = playerFeetY - BOTTOM_BOUNDARY;
		}
	}

	double panSpeed = 0.15;
	double difference = abs(m_TargetOffset.y - m_PrevTranslation.y);
	double epsilon = 0.01;

	if (difference < panSpeed)
	{
		panSpeed = difference;
	}
	if (panSpeed > epsilon)
	{
		if (m_TargetOffset.y > m_PrevTranslation.y)
		{
			translationY = m_PrevTranslation.y + panSpeed;
		}
		else
		{
			translationY = m_PrevTranslation.y - panSpeed;
		}
	}

	return translationY;
}

// TODO: Add yo functionality for certain levels
MATRIX3X2 Camera::GetViewMatrix(DOUBLE2 playerPos, int directionFacing, Level* levelPtr)
{
	DOUBLE2 newTranslation = GetOffset(playerPos, directionFacing, levelPtr);
	
	// NOTE: Uses negative position instead of .Inverse()
	MATRIX3X2 matCameraTranslation = MATRIX3X2::CreateTranslationMatrix(-newTranslation);

	return matCameraTranslation;
}

/* Paints extra debug info about the camera (Expects view matrix to be Game::matIdentity) */
void Camera::DEBUGPaint()
{
	double centerX = m_Width / 2;
	double x = 0;

	// HORIZONTAL
	GAME_ENGINE->SetColor(COLOR(10, 10, 250));
	x = MAX_X_OFFSET + HORIZONTAL_CUSHION_SIZE;
	GAME_ENGINE->DrawLine(x, 0, x, m_Height);
	x = m_Width - MAX_X_OFFSET - HORIZONTAL_CUSHION_SIZE;
	GAME_ENGINE->DrawLine(x, 0, x, m_Height);

	GAME_ENGINE->SetColor(COLOR(250, 50, 50));
	x = MAX_X_OFFSET;
	GAME_ENGINE->DrawLine(x, 0, x, m_Height);
	x = m_Width - MAX_X_OFFSET;
	GAME_ENGINE->DrawLine(x, 0, x, m_Height);


	// VERTICAL
	GAME_ENGINE->SetColor(COLOR(250, 200, 180));
	double y = TOP_BOUNDARY;
	GAME_ENGINE->DrawLine(0, y, m_Width, y);
	y = BOTTOM_BOUNDARY;
	GAME_ENGINE->DrawLine(0, y, m_Width, y);
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
