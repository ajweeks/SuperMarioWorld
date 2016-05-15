#pragma once

#include "BaseState.h"

class Level;

class GameState : public BaseState
{
public:
	GameState(StateManager* stateManagerPtr);
	virtual ~GameState();

	GameState(const GameState&) = delete;
	GameState& operator=(const GameState&) = delete;

	void Tick(double deltaTime);
	void Paint();

	void SetPaused(bool paused);

private:
	void Reset();
	void ResetMembers();

	Level *m_LevelPtr = nullptr;
	bool m_RenderDebugOverlay;
	
	bool m_InFrameByFrameMode = false;
};
