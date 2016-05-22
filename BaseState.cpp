#include "stdafx.h"

#include "BaseState.h"

BaseState::BaseState(StateManager* stateManagerPtr, StateType stateType) :
	m_StateManagerPtr(stateManagerPtr), m_StateType(stateType)
{
}

BaseState::~BaseState()
{
}

StateType BaseState::GetType()
{
	return m_StateType;
}

StateManager* BaseState::GetStateManagerPtr()
{
	return m_StateManagerPtr;
}
