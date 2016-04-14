#include "stdafx.h"

#include "Platform.h"
#include "Enumerations.h"

Platform::Platform(double left, double top, double right)
{
	m_Width = right - left;

	m_ActPtr = new PhysicsActor(DOUBLE2(left + m_Width / 2, top + HEIGHT / 2), 0, BodyType::STATIC);
	m_ActPtr->AddBoxFixture(m_Width, HEIGHT, 0.0);
	m_ActPtr->SetUserData(int(ActorId::PLATFORM));
	m_ActPtr->SetUserPointer(this);
}

Platform::~Platform()
{
	delete m_ActPtr;
}

void Platform::AddContactListener(ContactListener* listener)
{
	m_ActPtr->AddContactListener(listener);
}

bool Platform::Raycast(DOUBLE2 point1, DOUBLE2 point2, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef)
{
	return m_ActPtr->Raycast(point1, point2, intersectionRef, normalRef, fractionRef);
}

double Platform::GetWidth()
{
	return m_Width;
}

double Platform::GetHeight()
{
	return HEIGHT;
}
