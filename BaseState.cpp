#include "stdafx.h"

#include "BaseState.h"

BaseState::BaseState(StateManager* stateManagerPtr, STATE_TYPE stateType) :
	m_StateManagerPtr(stateManagerPtr), m_StateType(stateType)
{
}

BaseState::~BaseState()
{
}

STATE_TYPE BaseState::GetType()
{
	return m_StateType;
}