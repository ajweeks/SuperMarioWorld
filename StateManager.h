#pragma once

#include <stack>
#include "Game.h"

class BaseState;

class StateManager
{
public:
	StateManager(Game* gamePtr);
	virtual ~StateManager();

	StateManager(const StateManager&) = delete;
	StateManager& operator=(const StateManager&) = delete;

	void Tick(double deltaTime);
	void Paint();

	BaseState* CurrentState() const;
	Game* GetGamePtr() const;

	void SetState(BaseState* newStatePtr);

private:

	Game* m_GamePtr = nullptr;

	BaseState* m_CurrentStatePtr = nullptr;
};
