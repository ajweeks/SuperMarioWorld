#include "stdafx.h"

#include "Camera.h"
#include "Game.h"
#include "Player.h"
#include "Level.h"
#include "Keybindings.h"

const int Camera::DISTANCE_FROM_EDGE = 114;
const int Camera::HORIZONTAL_CUSHION_SIZE = 26;
const int Camera::TOP_BOUNDARY = 106;
const int Camera::BOTTOM_BOUNDARY = 150;
const int Camera::FRAMES_OF_TRANSITION = 25;

Camera::Camera(int width, int height, Level* levelPtr) : 
	WIDTH(width), HEIGHT(height)
{
	Reset();
	m_TransitionTimer = SMWTimer(FRAMES_OF_TRANSITION);
}

Camera::~Camera()
{
}

void Camera::Reset()
{
	m_PrevTranslation = DOUBLE2(0.0, 200.0);
	m_OffsetDirection = Direction::RIGHT;
	m_XOffset = DISTANCE_FROM_EDGE;
}

DOUBLE2 Camera::GetOffset(Level* levelPtr, double deltaTime)
{
#ifdef SMW_ENABLE_JUMP_TO
	if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::DEBUG_TELEPORT_PLAYER))
	{
		m_PrevTranslation.x = SMW_JUMP_TO_POS_X - WIDTH / 2.0;
		return m_PrevTranslation;
	}
#endif

	int xo = int(CalculateXOffset(levelPtr, deltaTime));
	int yo = int(CalculateYOffset(levelPtr, deltaTime));
	DOUBLE2 newOffset = DOUBLE2(xo, yo);

	Clamp(newTranslation, levelPtr);

	m_PrevTranslation = newTranslation;

	return newTranslation;
}

// Returns the x coordinate in world space of the camera's top left point
double Camera::CalculateXTranslation(Level* levelPtr, double deltaTime)
{
	Player* playerPtr = levelPtr->GetPlayer();
	DOUBLE2 playerPos = playerPtr->GetPosition();
	int playerFacingDr = playerPtr->GetDirectionFacing();
	int playerMovementDir = playerPtr->GetLinearVelocity().x >= 0.0 ? Direction::RIGHT : Direction::LEFT;
	double translationX = m_PrevTranslation.x;

	if (m_TransitionTimer.Tick())
	{
		// TODO: Fix this feature
		// Allow player to cancel transition by walking in opposite direction
		//if (m_OffsetDirection == Direction::LEFT)
		//{
		//	if (playerFacingDr == Direction::RIGHT)
		//	{
		//		m_TransitionTimer.SetPaused(true);
		//		return m_PrevTranslation.x;
		//	}
		//}
		//else if (m_OffsetDirection == Direction::RIGHT)
		//{
		//	if (playerFacingDr == Direction::LEFT)
		//	{
		//		m_TransitionTimer.SetPaused(true);
		//		return m_PrevTranslation.x;
		//	}
		//}

		// Calculate amount to transition
		const double maxDifference = (WIDTH - DISTANCE_FROM_EDGE * 2 + HORIZONTAL_CUSHION_SIZE);
		const double transitionElapsed = (double(m_TransitionTimer.FramesElapsed()) / double(m_TransitionTimer.TotalFrames()));

		// Transitioning from right to left
		if (m_OffsetDirection == Direction::LEFT)
		{
			m_XOffset = (DISTANCE_FROM_EDGE - HORIZONTAL_CUSHION_SIZE) + (transitionElapsed * maxDifference);
		}
		else if (m_OffsetDirection == Direction::RIGHT)
		{
			m_XOffset = -(DISTANCE_FROM_EDGE - HORIZONTAL_CUSHION_SIZE) + WIDTH - (transitionElapsed * maxDifference);
		}

		if (m_TransitionTimer.IsComplete())
		{
			if (m_OffsetDirection == Direction::LEFT)		m_XOffset = WIDTH - DISTANCE_FROM_EDGE;
			else if (m_OffsetDirection == Direction::RIGHT)	m_XOffset = DISTANCE_FROM_EDGE;

			return playerPos.x - m_XOffset;
		}

		translationX = playerPos.x - m_XOffset;
	}
	else
	{
		if (playerMovementDir == Direction::LEFT)
		{
			if (m_OffsetDirection == Direction::RIGHT) 
			{
				if (playerPos.x < m_PrevTranslation.x + DISTANCE_FROM_EDGE - HORIZONTAL_CUSHION_SIZE)
				{
					// The player crossed the boundary (blue line), transition to other direction
					m_OffsetDirection = Direction::LEFT;
					m_TransitionTimer.Start();
					return m_PrevTranslation.x;
				}
				else
				{
					return m_PrevTranslation.x;
				}
			}
			else if (m_OffsetDirection == Direction::LEFT)
			{
				if (playerPos.x < m_PrevTranslation.x + WIDTH - DISTANCE_FROM_EDGE)
				{
					m_XOffset = WIDTH - DISTANCE_FROM_EDGE;
				}
				else
				{
					return m_PrevTranslation.x;
				}
			}
		}
		else if (playerMovementDir == Direction::RIGHT)
		{
			if (m_OffsetDirection == Direction::LEFT)
			{
				if (playerPos.x > m_PrevTranslation.x + WIDTH - DISTANCE_FROM_EDGE + HORIZONTAL_CUSHION_SIZE)
				{
					// The player crossed the boundary (blue line), transition to other direction
					m_OffsetDirection = Direction::RIGHT;
					m_TransitionTimer.Start();
					return m_PrevTranslation.x;
				}
				else
				{
					return m_PrevTranslation.x;
				}
			}
			else if (m_OffsetDirection == Direction::RIGHT)
			{
				if (playerPos.x > m_PrevTranslation.x + DISTANCE_FROM_EDGE)
				{
					m_XOffset = DISTANCE_FROM_EDGE;
				}
				else
				{
					return m_PrevTranslation.x;
				}
			}
		}

		translationX = playerPos.x - m_XOffset;
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

// TODO: Fix me :(
// NOTE: The camera only takes into account the player's y pos if they are running at full speed or have climbed a beanstalk
// Returns the y coordinate in world space of the camera's top left point
double Camera::CalculateYTranslation(Level* levelPtr, double deltaTime)
{
	double targetYOffset = 550;
	double translationY = m_PrevTranslation.y;
	Player* playerPtr = levelPtr->GetPlayer();
	double playerFeetY = playerPtr->GetPosition().y + playerPtr->GetHeight() / 2.0;

	/*if (playerPtr->GetAnimationState() == Player::AnimationState::CLIMBING)
	{
		m_UsingConstantYO = false;
		if (playerPtr->GetPosition().y - m_PrevTranslation.y > BOTTOM_BOUNDARY)
		{
			targetYOffset = playerPtr->GetPosition().y - BOTTOM_BOUNDARY;
		}
		else if (playerPtr->GetPosition().y - m_PrevTranslation.y < TOP_BOUNDARY)
		{
			targetYOffset = playerPtr->GetPosition().y - TOP_BOUNDARY;
		}
	}

	if (m_UsingConstantYO == false)
	{
		if (playerPtr->IsOnGround())
		{
			if (playerFeetY > 335)
			{
				m_UsingConstantYO = true;
				targetYOffset = 550;
			}
			else
			{
				targetYOffset = playerFeetY - BOTTOM_BOUNDARY;
			}
		}
		else if (playerFeetY - m_PrevTranslation.y > BOTTOM_BOUNDARY)
		{
			translationY = playerFeetY - BOTTOM_BOUNDARY;
			return translationY;
		}
	}*/

	double panSpeed = 40;
	double difference = abs(targetYOffset - m_PrevTranslation.y);
	double epsilon = 0.01;

	if (difference < panSpeed)
	{
		panSpeed = difference;
	}
	if (panSpeed > epsilon)
	{
		if (targetYOffset > m_PrevTranslation.y)
		{
			translationY = m_PrevTranslation.y + panSpeed * deltaTime;
		}
		else
		{
			translationY = m_PrevTranslation.y - panSpeed * deltaTime;
		}
	}

	return translationY;
}

void Camera::CalculateViewMatrix(Level* levelPtr, double deltaTime)
{
	DOUBLE2 newTranslation = GetOffset(levelPtr, deltaTime);
	
	m_MatTranslation = MATRIX3X2::CreateTranslationMatrix(-newTranslation);
}

MATRIX3X2 Camera::GetViewMatrix()
{
	return m_MatTranslation;
}

/* Paints extra debug info about the camera (Expects view matrix to be Game::matIdentity) */
void Camera::DEBUGPaint()
{
	double centerX = WIDTH / 2.0;
	double x = 0;

	GAME_ENGINE->SetColor(COLOR(10, 10, 250));
	x = DISTANCE_FROM_EDGE - HORIZONTAL_CUSHION_SIZE;
	GAME_ENGINE->DrawLine(x, 0, x, HEIGHT);
	x = WIDTH - DISTANCE_FROM_EDGE + HORIZONTAL_CUSHION_SIZE;
	GAME_ENGINE->DrawLine(x, 0, x, HEIGHT);

	GAME_ENGINE->SetColor(COLOR(250, 50, 50));
	x = DISTANCE_FROM_EDGE;
	GAME_ENGINE->DrawLine(x, 0, x, HEIGHT);
	x = WIDTH - DISTANCE_FROM_EDGE;
	GAME_ENGINE->DrawLine(x, 0, x, HEIGHT);


	GAME_ENGINE->SetColor(COLOR(250, 200, 180));
	double y = TOP_BOUNDARY;
	GAME_ENGINE->DrawLine(0, y, WIDTH, y);
	y = BOTTOM_BOUNDARY;
	GAME_ENGINE->DrawLine(0, y, WIDTH, y);
}

void Camera::Clamp(DOUBLE2& posRef, Level* levelPtr)
{
	if (posRef.x < 0) posRef.x = 0;
	else if (posRef.x > levelPtr->GetWidth() - WIDTH) posRef.x = levelPtr->GetWidth() - WIDTH;

	if (posRef.y < 0) posRef.y = 0;
	else if (posRef.y > levelPtr->GetHeight() - HEIGHT) posRef.y = levelPtr->GetHeight() - HEIGHT;
}
