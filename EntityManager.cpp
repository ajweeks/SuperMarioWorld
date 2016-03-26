#include "stdafx.h"
#include "EntityManager.h"
#include "Entity.h"

#define GAME_ENGINE (GameEngine::GetSingleton())

EntityManager *EntityManager::m_SingletonPtr = nullptr;

EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
}

void EntityManager::Unload()
{
	delete m_SingletonPtr;
}

EntityManager* EntityManager::GetInstance()
{
	if (m_SingletonPtr == nullptr)
	{
		m_SingletonPtr = new EntityManager();
	}

	return m_SingletonPtr;
}

void EntityManager::Tick(double deltaTime, Level *levelPtr)
{
	for (size_t i = 0; i < m_EntityPtrArr.size(); ++i)
	{
		m_EntityPtrArr[i]->Tick(deltaTime, levelPtr);
	}
}

void EntityManager::Paint()
{
	for (size_t i = 0; i < m_EntityPtrArr.size(); ++i)
	{
		m_EntityPtrArr[i]->Paint();
	}
}

void EntityManager::Add(Entity* entity)
{
	for (size_t i = 0; i < m_EntityPtrArr.size(); ++i)
	{
		if (m_EntityPtrArr[i] == nullptr)
		{
			m_EntityPtrArr[i] = entity;
			return;
		}
	}

	m_EntityPtrArr.push_back(entity);
}

bool EntityManager::Remove(Entity* entity)
{
	for (size_t i = 0; i < m_EntityPtrArr.size(); ++i)
	{
		if (m_EntityPtrArr[i] == entity)
		{
			delete m_EntityPtrArr[i];
			m_EntityPtrArr[i] = nullptr;
			return true;
		}
	}
	return false;
}

bool EntityManager::Remove(int entityIndex)
{
	assert(entityIndex >= 0 && entityIndex < int(m_EntityPtrArr.size()));

	delete m_EntityPtrArr[entityIndex];
	m_EntityPtrArr[entityIndex] = nullptr;

	return true;
}

void EntityManager::RemoveAll()
{
	for (size_t i = 0; i < m_EntityPtrArr.size(); ++i)
	{
		Remove(i);
	}
}