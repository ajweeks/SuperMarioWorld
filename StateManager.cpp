#include "stdafx.h"

#include "StateManager.h"
#include "BaseState.h"
#include "MainMenuState.h"
#include "GameState.h"

StateManager::StateManager(Game* gamePtr) : 
	m_GamePtr(gamePtr)
{
#if SMW_SKIP_MAIN_MENU
	m_CurrentStatePtr = new GameState(this);
#else
	m_CurrentStatePtr = new MainMenuState(this);
#endif
}

StateManager::~StateManager()
{
	delete m_CurrentStatePtr;
}

void StateManager::Tick(double deltaTime)
{
	CurrentState()->Tick(deltaTime);
}

void StateManager::Paint()
{
	CurrentState()->Paint();
}

BaseState* StateManager::CurrentState() const
{
	return m_CurrentStatePtr;
}

void StateManager::SetState(BaseState* newStatePtr)
{
	if (m_CurrentStatePtr != nullptr) delete m_CurrentStatePtr;
	m_CurrentStatePtr = newStatePtr;
}

Game* StateManager::GetGamePtr() const
{
	return m_GamePtr;
}
