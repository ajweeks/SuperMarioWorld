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

	inline BaseState* CurrentState();

	void PushState(BaseState* newStatePtr);
	void PopState();

	Game* GetGamePtr();

private:

	Game* m_GamePtr = nullptr;

	std::stack<BaseState*> m_StatesPtrStack;
};
