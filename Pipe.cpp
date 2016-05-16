#include "stdafx.h"

#include "Pipe.h"
#include "Enumerations.h"
#include "Level.h"
#include "Player.h"

Pipe::Pipe(DOUBLE2 topLeft, DOUBLE2 bottomRight, Level* levelPtr, Orientation orientation, bool canAccess) :
	m_CanAccess(canAccess),
	m_Orientation(orientation)
{
	m_Bounds = RECT2(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
	double width = bottomRight.x - topLeft.x;
	double height = bottomRight.y - topLeft.y;
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
	if (m_CanAccess == false) return false;
	if (playerPtr->IsAirborne()) return false; // The player must be on the ground to enter a pipe

	DOUBLE2 pipePos = m_ActPtr->GetPosition();
	DOUBLE2 playerPos = playerPtr->GetPosition();

	double pipeWidth = m_Bounds.right - m_Bounds.left;
	double pipeHeight = m_Bounds.bottom - m_Bounds.top;

	switch (m_Orientation)
	{
	case Orientation::LEFT:  return (playerPos.x < pipePos.x - pipeWidth / 2);
	case Orientation::RIGHT: return (playerPos.x > pipePos.x + pipeWidth / 2);
	case Orientation::UP:	 return (playerPos.y < pipePos.y - pipeHeight / 2);
	case Orientation::DOWN:  return (playerPos.y > pipePos.y + pipeHeight / 2);
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
	default: return "UNKNOWN PIPE ORIENTATION!!!!";
	}
}
