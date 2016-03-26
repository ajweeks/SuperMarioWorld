#include "stdafx.h"
#include "Entity.h"
#include "SpriteSheet.h"

#define GAME_ENGINE (GameEngine::GetSingleton())

Entity::Entity(DOUBLE2& posRef, SpriteSheet *spriteSheetPtr, BodyType bodyType, void* userPointer, DOUBLE2& initialVelRef)
	: m_SpriteSheetPtr(spriteSheetPtr)
{
	m_ActPtr = new PhysicsActor(posRef, 0, bodyType);
	if (userPointer != nullptr) m_ActPtr->SetUserPointer(userPointer);
	m_ActPtr->SetLinearVelocity(initialVelRef);
}

Entity::~Entity()
{
	delete m_ActPtr;
}

// TODO: Are these definitions required?
//void Entity::Tick(double deltaTime) {}
//void Entity::Paint() {}
