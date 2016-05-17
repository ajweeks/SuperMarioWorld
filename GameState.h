#pragma once

#include "BaseState.h"

class Level;
class Pipe;
struct SessionInfo;

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

	void EnterUnderground(SessionInfo sessionInfo, Pipe* pipePtr);
	void LeaveUnderground(SessionInfo sessionInfo, Pipe* pipeEnteredPtr);

private:
	void Reset();
	void ResetMembers();

	Level* m_CurrentLevelPtr = nullptr;
	Level* m_LevelOverworldPtr = nullptr;
	Level* m_LevelUndergroundPtr = nullptr; // This is used so that we can keep the overworld 

	bool m_RenderDebugOverlay;
	
	bool m_InFrameByFrameMode = false;
};
