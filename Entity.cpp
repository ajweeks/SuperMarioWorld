#include "stdafx.h"

#include "Entity.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

#define GAME_ENGINE (GameEngine::GetSingleton())

Entity::Entity(DOUBLE2& posRef, BodyType bodyType, 
	Level* levelPtr, void* userPointer, DOUBLE2& initialVelRef) :
	m_LevelPtr(levelPtr)
{
	m_ActPtr = new PhysicsActor(posRef, 0, bodyType);
	if (userPointer != nullptr)
	{
		m_ActPtr->SetUserPointer(userPointer);
	}
	m_ActPtr->SetLinearVelocity(initialVelRef);
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

void Entity::AddContactListener(ContactListener* listener)
{
	m_ActPtr->AddContactListener(listener);
}

DOUBLE2 Entity::GetPosition()
{
	return m_ActPtr->GetPosition();
}