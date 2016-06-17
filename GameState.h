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

	void SetPaused(bool paused, bool pauseSongs);
	bool ShowingSessionInfo() const;

	// Spawn at given pipe
	void EnterNewLevel(Pipe* spawningPipePtr, SessionInfo sessionInfo);
	// Spawn at beginning of level
	void EnterNewLevel(int levelIndex, SessionInfo sessionInfo, Pipe* spawningPipePtr = nullptr);

private:
	void Reset();
	void ResetMembers();
	
	Level* m_CurrentLevelPtr = nullptr;
	bool m_ShowingSessionInfo;
	bool m_RenderDebugOverlay;
	bool m_InFrameByFrameMode;
};
