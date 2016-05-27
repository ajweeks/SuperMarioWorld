#include "stdafx.h"

#include "StateManager.h"
#include "BaseState.h"
#include "MainMenuState.h"
#include "GameState.h"

StateManager::StateManager(Game* gamePtr) : 
	m_GamePtr(gamePtr)
{
#if SMW_SKIP_MAIN_MENU
	m_StatesPtrStack.push(new GameState(this));
#else
	m_StatesPtrStack.push(new MainMenuState(this));
#endif
}

StateManager::~StateManager()
{
	for (size_t i = 0; i < m_StatesPtrStack.size(); ++i)
	{
		delete m_StatesPtrStack.top();
		m_StatesPtrStack.pop();
	}
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
	return m_StatesPtrStack.top();
}

void StateManager::PushState(BaseState* newStatePtr)
{
	m_StatesPtrStack.push(newStatePtr);
}

void StateManager::PopState()
{
	if (m_StatesPtrStack.size() == 1) return;

	delete CurrentState();
	m_StatesPtrStack.pop();
}

Game* StateManager::GetGamePtr()
{
	return m_GamePtr;
}
