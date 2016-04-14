#include "stdafx.h"

#include "Pipe.h"
#include "Enumerations.h"

Pipe::Pipe(DOUBLE2 topLeft, DOUBLE2 bottomRight, bool canAccess) : m_CanAccess(canAccess)
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