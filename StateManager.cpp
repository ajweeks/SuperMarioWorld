#include "stdafx.h"

#include "StateManager.h"
#include "BaseState.h"
#include "MainMenuState.h"
#include "GameState.h"


StateManager::StateManager(Game* gamePtr) : 
	m_GamePtr(gamePtr)
{
	m_StatesPtrArr.push_back(new MainMenuState(this));
}

StateManager::~StateManager()
{
	for (size_t i = 0; i < m_StatesPtrArr.size(); ++i)
	{
		delete m_StatesPtrArr[i];
	}
	m_StatesPtrArr.clear();
}

void StateManager::Tick(double deltaTime)
{
	CurrentState()->Tick(deltaTime);
}

void StateManager::Paint()
{
	CurrentState()->Paint();
}

BaseState* StateManager::CurrentState()
{
	return m_StatesPtrArr[m_StatesPtrArr.size() - 1];
}

void StateManager::PushState(BaseState* newStatePtr)
{
	m_StatesPtrArr.push_back(newStatePtr);
}

BaseState* StateManager::PopState()
{
	if (m_StatesPtrArr.size() == 1) return nullptr;

	BaseState* statePtr = CurrentState();
	m_StatesPtrArr.pop_back();
	return statePtr;
}

Game* StateManager::GetGamePtr()
{
	return m_GamePtr;
}
