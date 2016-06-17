#pragma once

#include "Enumerations.h"
#include "AnimationInfo.h"

class SpriteSheet;
class Level;

class Entity
{
public:
	Entity(DOUBLE2 centerPos, BodyType bodyType,
		Level* levelPtr, ActorId actorId, void* userPointer = nullptr, DOUBLE2& initialVelRef = DOUBLE2());
	virtual ~Entity();

	Entity(const Entity&) = delete;
	Entity& operator=(const Entity&) = delete;

	// NOTE: Returns true when this entity should be removed
	virtual void Tick(double deltaTime) = 0;
	virtual void Paint() = 0;
	virtual bool IsPaused() const;
	virtual void SetPaused(bool paused);

	DOUBLE2 GetPosition();

	void AddContactListener(ContactListener* listener);

	virtual bool Raycast(DOUBLE2 point1, DOUBLE2 point2, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef);

protected:
	PhysicsActor* m_ActPtr = nullptr;
	Level* m_LevelPtr = nullptr;
	AnimationInfo m_AnimInfo;
};
