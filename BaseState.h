#pragma once

#include "CountdownTimer.h"

class StateManager;

enum class STATE_TYPE
{
	MAIN_MENU, GAME
};

class BaseState
{
public:
	BaseState(StateManager* stateManagerPtr, STATE_TYPE stateType);
	virtual ~BaseState();

	BaseState(const BaseState&) = delete;
	BaseState& operator=(const BaseState&) = delete;

	virtual void Tick(double deltaTime) = 0;
	virtual void Paint() = 0;

	STATE_TYPE GetType();
	StateManager* GetStateManagerPtr();

protected:
	StateManager* m_StateManagerPtr = nullptr;
	STATE_TYPE m_StateType;

};
