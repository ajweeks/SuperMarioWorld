#pragma once

class Entity;
class Level;

/* 
	NOTE: This class has an array of pointers to Entity objects.
		 It is *not* responsible for the deletion of these objects,
		 but rather to simply update and render them. It provides an
		 interface for abstracting away adding and removing entities.

		 EntityManager::Unload() simply deletes the singleton pointer.
*/
class EntityManager
{
public:
	virtual ~EntityManager();

	EntityManager(const EntityManager&) = delete;
	EntityManager& operator=(const EntityManager&) = delete;

	static EntityManager* GetInstance();
	static void Unload();

	void Tick(double deltaTime, Level *levelPtr);
	void Paint();

	/* Returns whether or not the specified entity was able to be found (and thus removed) */
	bool Remove(Entity* entity);
	bool Remove(int entityIndex);
	void RemoveAll();

	void Add(Entity* entity);

private:
	EntityManager();
	static EntityManager *m_SingletonPtr;

	std::vector<Entity*> m_EntityPtrArr;
};
