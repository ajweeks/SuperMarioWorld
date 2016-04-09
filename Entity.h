#pragma once

#include "Enumerations.h"

class EntityManager;
class SpriteSheet;
class Level;

class Entity
{
public:
	Entity(DOUBLE2& posRef, SpriteSheet* spriteSheetPtr, BodyType bodyType, 
		Level* levelPtr, void* userPointer = nullptr, DOUBLE2& initialVelRef = DOUBLE2());
	virtual ~Entity();

	Entity& operator=(const Entity&) = delete;
	Entity(const Entity&) = delete;

	// NOTE: Returns true when this entity should be removed
	virtual void Tick(double deltaTime) = 0;
	virtual void Paint() = 0;
	virtual void SetPaused(bool paused);

	DOUBLE2 GetPosition();

	void AddContactListener(ContactListener* listener);

	bool Raycast(DOUBLE2 point1, DOUBLE2 point2, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef);

protected:
	// TODO: Remove spritesheet pointer, only the player class really needs it
	SpriteSheet* m_SpriteSheetPtr = nullptr;
	PhysicsActor* m_ActPtr = nullptr;
	Level* m_LevelPtr = nullptr;
	ANIMATION_INFO m_AnimInfo;
};
