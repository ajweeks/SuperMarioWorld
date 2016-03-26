#pragma once

class EntityManager;
class SpriteSheet;
class Level;

class Entity
{
public:
	Entity(DOUBLE2& posRef, SpriteSheet *spriteSheetPtr, BodyType bodyType, void* userPointer = nullptr, DOUBLE2& initialVelRef = DOUBLE2());
	virtual ~Entity();

	Entity& operator=(const Entity&) = delete;
	Entity(const Entity&) = delete;

	virtual void Tick(double deltaTime, Level *levelPtr) = 0;
	virtual void Paint() = 0;

protected:
	PhysicsActor* m_ActPtr = nullptr;
	SpriteSheet* m_SpriteSheetPtr = nullptr;
};
