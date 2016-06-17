#include "stdafx.h"

#include "Camera.h"
#include "Game.h"
#include "Player.h"
#include "Level.h"
#include "Keybindings.h"

const int Camera::DISTANCE_FROM_EDGE = 114;
const int Camera::HORIZONTAL_CUSHION_SIZE = 26;
const int Camera::TOP_BOUNDARY = 106;
const int Camera::TOP_BOUNDARY_LOW = 125;
const int Camera::BOTTOM_BOUNDARY = 150;
const int Camera::FRAMES_OF_TRANSITION = 25;
const int Camera::DEFAULT_Y_OFFSET = 160;

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
	m_PrevOffset = DOUBLE2(0.0, DEFAULT_Y_OFFSET);
	m_OffsetDirection = Direction::RIGHT;
	m_XOffset = DISTANCE_FROM_EDGE;
}

DOUBLE2 Camera::GetOffset(Level* levelPtr, double deltaTime)
{
	if (Game::DEBUG_TELEPORT_PLAYER_TO_POSITION != -1)
	{
		if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::DEBUG_TELEPORT_PLAYER))
		{
			m_PrevOffset.x = Game::DEBUG_TELEPORT_PLAYER_TO_POSITION - WIDTH / 2.0;
			return m_PrevOffset;
		}
	}

	int xo = int(CalculateXOffset(levelPtr, deltaTime));
	int yo = int(CalculateYOffset(levelPtr, deltaTime));
	DOUBLE2 newOffset = DOUBLE2(xo, yo);

	Clamp(newOffset, levelPtr);

	m_PrevOffset = newOffset;

	return newOffset;
}

// Returns the x coordinate in world space of the camera's top left point
double Camera::CalculateXOffset(Level* levelPtr, double deltaTime)
{
	Player* playerPtr = levelPtr->GetPlayer();
	DOUBLE2 playerPos = playerPtr->GetPosition();
	int playerFacingDr = playerPtr->GetDirectionFacing();
	int playerMovementDir = playerPtr->GetLinearVelocity().x >= 0.0 ? Direction::RIGHT : Direction::LEFT;
	double translationX = m_PrevOffset.x;

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
				if (playerPos.x < m_PrevOffset.x + DISTANCE_FROM_EDGE - HORIZONTAL_CUSHION_SIZE)
				{
					// The player crossed the boundary (blue line), transition to other direction
					m_OffsetDirection = Direction::LEFT;
					m_TransitionTimer.Start();
					return m_PrevOffset.x;
				}
				else
				{
					return m_PrevOffset.x;
				}
			}
			else if (m_OffsetDirection == Direction::LEFT)
			{
				if (playerPos.x < m_PrevOffset.x + WIDTH - DISTANCE_FROM_EDGE)
				{
					m_XOffset = WIDTH - DISTANCE_FROM_EDGE;
				}
				else
				{
					return m_PrevOffset.x;
				}
			}
		}
		else if (playerMovementDir == Direction::RIGHT)
		{
			if (m_OffsetDirection == Direction::LEFT)
			{
				if (playerPos.x > m_PrevOffset.x + WIDTH - DISTANCE_FROM_EDGE + HORIZONTAL_CUSHION_SIZE)
				{
					// The player crossed the boundary (blue line), transition to other direction
					m_OffsetDirection = Direction::RIGHT;
					m_TransitionTimer.Start();
					return m_PrevOffset.x;
				}
				else
				{
					return m_PrevOffset.x;
				}
			}
			else if (m_OffsetDirection == Direction::RIGHT)
			{
				if (playerPos.x > m_PrevOffset.x + DISTANCE_FROM_EDGE)
				{
					m_XOffset = DISTANCE_FROM_EDGE;
				}
				else
				{
					return m_PrevOffset.x;
				}
			}
		}

		translationX = playerPos.x - m_XOffset;
	}
	
	return translationX;
}

/*

Only increase the yo when the player is running full speed and jumping, or climbing upwards
Only decrease the yo when the player is falling near the bottom of the screen, or climbing downwards

*/

// NOTE: The camera only takes into account the player's y pos if they are running at full speed or have climbed a beanstalk
// Returns the y coordinate in world space of the camera's top left point
double Camera::CalculateYOffset(Level* levelPtr, double deltaTime)
{
	Player* playerPtr = levelPtr->GetPlayer();
	const Player::AnimationState playerAnimationState = playerPtr->GetAnimationState();
	const bool playerIsClimbing = playerAnimationState == Player::AnimationState::CLIMBING;
	const bool playerIsRunning = playerPtr->IsRunning();

	if (!playerIsClimbing && 
		!playerIsRunning &&
		m_PrevOffset.y == DEFAULT_Y_OFFSET)
	{
		return DEFAULT_Y_OFFSET;
	}

	double yOffset = m_PrevOffset.y;
	const double playerFeetY = playerPtr->GetPosition().y + playerPtr->GetHeight() / 2.0;

	if (playerIsClimbing)
	{
		if (playerFeetY - yOffset > BOTTOM_BOUNDARY)
		{
			yOffset = playerFeetY - BOTTOM_BOUNDARY;
		}
		else if (playerFeetY - yOffset < TOP_BOUNDARY)
		{
			yOffset = playerFeetY - TOP_BOUNDARY;
		}
	}
	else if (playerIsRunning)
	{
		if (playerFeetY - yOffset < TOP_BOUNDARY_LOW)
		{
			yOffset = playerFeetY - TOP_BOUNDARY_LOW;
		}
	}

	if (m_PrevOffset.y != DEFAULT_Y_OFFSET)
	{
		if (playerPtr->IsOnGround())
		{
			if (playerFeetY > 335) // We hit ground level
			{
				yOffset = DEFAULT_Y_OFFSET;
			}
			else
			{
				yOffset = playerFeetY - BOTTOM_BOUNDARY;
			}
		}
		else if (playerFeetY - yOffset > BOTTOM_BOUNDARY) // Prevent the player from falling off the bottom of the screen
		{
			yOffset = playerFeetY - BOTTOM_BOUNDARY;
		}
	}

	return yOffset;
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

	GAME_ENGINE->SetColor(COLOR(170, 140, 250));
	y = TOP_BOUNDARY_LOW;
	GAME_ENGINE->DrawLine(0, y, WIDTH, y);
}

void Camera::Clamp(DOUBLE2& posRef, Level* levelPtr)
{
	if (posRef.x < 0) posRef.x = 0;
	else if (posRef.x > levelPtr->GetWidth() - WIDTH) posRef.x = levelPtr->GetWidth() - WIDTH;

	if (posRef.y < 0) posRef.y = 0;
	else if (posRef.y > levelPtr->GetHeight() - HEIGHT) posRef.y = levelPtr->GetHeight() - HEIGHT;
}
