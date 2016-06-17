#include "stdafx.h"

#include "Entity.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

Entity::Entity(DOUBLE2 centerPos, BodyType bodyType,
	Level* levelPtr, ActorId actorId, void* userPointer, DOUBLE2& initialVelRef) :
	m_LevelPtr(levelPtr)
{
	m_ActPtr = new PhysicsActor(centerPos, 0, bodyType);
	m_ActPtr->SetUserData(int(actorId));
	if (userPointer != nullptr)
	{
		m_ActPtr->SetUserPointer(userPointer);
	}
	if (initialVelRef != DOUBLE2()) m_ActPtr->SetLinearVelocity(initialVelRef);
	m_ActPtr->SetFixedRotation(true);
}

Entity::~Entity()
{
	delete m_ActPtr;
}

bool Entity::Raycast(DOUBLE2 point1, DOUBLE2 point2, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef)
{
	return m_ActPtr->Raycast(point1, point2, intersectionRef, normalRef, fractionRef);
}

void Entity::SetPaused(bool paused)
{
	m_ActPtr->SetActive(!paused);
}

bool Entity::IsPaused() const
{
	return m_ActPtr->IsActive() == false;
}

void Entity::AddContactListener(ContactListener* listener)
{
	m_ActPtr->AddContactListener(listener);
}

DOUBLE2 Entity::GetPosition()
{
	return m_ActPtr->GetPosition();
}
