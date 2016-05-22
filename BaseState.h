#pragma once

#include "CountdownTimer.h"

class StateManager;

enum class StateType
{
	MAIN_MENU, GAME
};

class BaseState
{
public:
	BaseState(StateManager* stateManagerPtr, StateType stateType);
	virtual ~BaseState();

	BaseState(const BaseState&) = delete;
	BaseState& operator=(const BaseState&) = delete;

	virtual void Tick(double deltaTime) = 0;
	virtual void Paint() = 0;

	StateType GetType();
	StateManager* GetStateManagerPtr();

protected:
	StateManager* m_StateManagerPtr = nullptr;
	StateType m_StateType;

};
