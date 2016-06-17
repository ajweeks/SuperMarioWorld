#include "stdafx.h"

#include "Pipe.h"
#include "Enumerations.h"
#include "Level.h"
#include "Player.h"

Pipe::Pipe(DOUBLE2 topLeft, DOUBLE2 bottomRight, Level* levelPtr, Orientation orientation,
	int index, int warpLevelIndex, int warpPipeIndex) :
	m_Orientation(orientation),
	m_Index(index)
{
	if (warpLevelIndex != -1)
	{
		m_WarpPipeIndex = warpPipeIndex;
		m_WarpLevelIndex = warpLevelIndex;
		m_IsWarpPipe = true;
	}
	else
	{
		m_IsWarpPipe = false;
	}

	m_Bounds = RECT2(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
	const double width = bottomRight.x - topLeft.x;
	const double height = bottomRight.y - topLeft.y;
	m_ActPtr = new PhysicsActor(topLeft + DOUBLE2(width / 2, height / 2), 0, BodyType::STATIC);
	m_ActPtr->AddBoxFixture(width, height, 0.0);
	m_ActPtr->SetUserData(int(ActorId::PIPE));
	m_ActPtr->SetUserPointer(this);
}

Pipe::~Pipe()
{
	delete m_ActPtr;
}

void Pipe::AddContactListener(ContactListener* listener)
{
	m_ActPtr->AddContactListener(listener);
}

bool Pipe::Raycast(DOUBLE2 point1, DOUBLE2 point2, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef)
{
	return m_ActPtr->Raycast(point1, point2, intersectionRef, normalRef, fractionRef);
}

Pipe::Orientation Pipe::GetOrientation()
{
	return m_Orientation;
}

bool Pipe::IsPlayerInPositionToEnter(Player* playerPtr)
{
	if (m_IsWarpPipe == false) return false;
	if (playerPtr->IsAirborne()) return false; // The player must be on the ground to enter a pipe

	const DOUBLE2 pipePos = m_ActPtr->GetPosition();
	const DOUBLE2 playerPos = playerPtr->GetPosition();

	const double pipeWidth = m_Bounds.right - m_Bounds.left;
	const double pipeHeight = m_Bounds.bottom - m_Bounds.top;

	const double playerWidth = playerPtr->GetWidth();
	const double playerHeight = playerPtr->GetHeight();

	double minDistFromCenter = pipeWidth / 5.0;
	
	switch (m_Orientation)
	{
	case Orientation::LEFT:  return (playerPos.x + playerWidth / 2 < pipePos.x - pipeWidth / 2) && 
									(playerPos.y - playerHeight / 2 > pipePos.y - pipeHeight / 2);

	case Orientation::RIGHT: return (playerPos.x - playerPtr->GetWidth() / 2 > pipePos.x + pipeWidth / 2) &&
									(playerPos.y - playerHeight / 2> pipePos.y - pipeHeight / 2);

	case Orientation::UP:	 return (playerPos.y + playerPtr->GetHeight() / 2 < pipePos.y - pipeHeight / 2) &&
									(abs(playerPos.x - pipePos.x) < minDistFromCenter);

	case Orientation::DOWN:  return (playerPos.y > pipePos.y + pipeHeight / 2) && 
									(abs(playerPos.x - pipePos.x) < minDistFromCenter);

	default: return false;
	}
}

Pipe::Orientation Pipe::StringToOrientation(std::string orientationStr)
{
	if (!orientationStr.compare("Left")) return Orientation::LEFT;
	else if (!orientationStr.compare("Right")) return Orientation::RIGHT;
	else if (!orientationStr.compare("Up")) return Orientation::UP;
	else if (!orientationStr.compare("Down")) return Orientation::DOWN;
	else return Orientation::NONE;
}

std::string Pipe::OrientationToString(Orientation orientation)
{
	switch (orientation)
	{
	case Orientation::LEFT: return "Left";
	case Orientation::RIGHT: return "Right";
	case Orientation::UP: return "Up";
	case Orientation::DOWN: return "Down";
	default: return "Unknown Pipe orientation!!!!" + int(orientation);
	}
}

int Pipe::GetIndex()
{
	return m_Index;
}

int Pipe::GetWarpLevelIndex()
{
	return m_WarpLevelIndex;
}

int Pipe::GetWarpPipeIndex()
{
	return m_WarpPipeIndex;
}

// Where to warp the player to when they come through this pipe
DOUBLE2 Pipe::GetWarpToPosition()
{
	DOUBLE2 offset(0.0, 0.0);

	const double width = m_Bounds.right - m_Bounds.left;
	const double height = m_Bounds.bottom - m_Bounds.top;

	// How far into the pipe the player should start at
	const double xInsetAmount = 10;
	const double yInsetAmount = 20;

	switch (m_Orientation)
	{
	case Orientation::LEFT:		offset = DOUBLE2(-width / 2.0 + xInsetAmount, 0);  break;
	case Orientation::RIGHT:	offset = DOUBLE2(width / 2.0 - xInsetAmount, 0);   break;
	case Orientation::UP:		offset = DOUBLE2(0, -height / 2.0 + yInsetAmount); break;
	case Orientation::DOWN:		offset = DOUBLE2(0, height / 2.0 - yInsetAmount);  break;
	}

	return m_ActPtr->GetPosition() + offset;
}
