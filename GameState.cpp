#include "stdafx.h"

#include "GameState.h"
#include "Game.h"
#include "GameSession.h"
#include "StateManager.h"
#include "LevelProperties.h"
#include "SpriteSheetManager.h"
#include "SoundManager.h"
#include "SessionInfo.h"
#include "Pipe.h"
#include "Keybindings.h"

GameState::GameState(StateManager* stateManagerPtr) :
	BaseState(stateManagerPtr, StateType::GAME)
{
	GameSession::ReadSessionInfoFromFile();

	int levelIndex = 0; // Start the player in level 0
	m_CurrentLevelPtr = new Level(m_StateManagerPtr->GetGamePtr(), this, LevelProperties::Get(levelIndex));

	ResetMembers();
	
	GameSession::RecordStartSessionInfo(m_CurrentLevelPtr);
}

GameState::~GameState()
{
	GameSession::WriteSessionInfoToFile(m_CurrentLevelPtr);
	delete m_CurrentLevelPtr;
}

void GameState::Reset()
{
	m_CurrentLevelPtr->Reset();

	m_StateManagerPtr->GetGamePtr()->Reset();

	ResetMembers();
}

void GameState::ResetMembers()
{
	m_ShowingSessionInfo = false;
	m_InFrameByFrameMode = false;
	m_RenderDebugOverlay = false;
	GAME_ENGINE->EnablePhysicsDebugRendering(m_RenderDebugOverlay);
}

void GameState::Tick(double deltaTime)
{
	if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::TOGGLE_INFO_OVERLAY))
	{
		m_ShowingSessionInfo = !m_ShowingSessionInfo;
		SetPaused(m_ShowingSessionInfo, true);
	}

	if (m_ShowingSessionInfo)
	{
		GameSession::Tick(deltaTime);
	}

	if (m_InFrameByFrameMode && m_CurrentLevelPtr->IsPaused() == false)
	{
		// We've advanced by one frame, now we need to pause again
		m_CurrentLevelPtr->SetPaused(true, true);
	}
		
	if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::DEBUG_FRAME_BY_FRAME_ADVANCE))
	{
		if (m_InFrameByFrameMode)
		{
			// Advance by one frame
			m_CurrentLevelPtr->SetPaused(false, false);
		}
		else
		{
			m_InFrameByFrameMode = true;
			m_CurrentLevelPtr->SetPaused(true, true);
		}
	}
	else if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::START_BUTTON) && m_InFrameByFrameMode)
	{
		m_InFrameByFrameMode = false;
		m_CurrentLevelPtr->SetPaused(false, true);
	}

	if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::DEBUG_QUICK_RESET))
	{
		Reset();
	}

	if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::DEBUG_TOGGLE_PHYSICS_RENDERING))
	{
		m_RenderDebugOverlay = !m_RenderDebugOverlay;
		GAME_ENGINE->EnablePhysicsDebugRendering(m_RenderDebugOverlay);
	}

	m_CurrentLevelPtr->Tick(deltaTime);
}

void GameState::Paint()
{
	m_CurrentLevelPtr->Paint();

	if (m_ShowingSessionInfo)
	{
		GameSession::Paint();
	}
}

void GameState::SetPaused(bool paused, bool pauseSongs)
{
	m_CurrentLevelPtr->SetPaused(paused, pauseSongs);
}

void GameState::EnterNewLevel(Pipe* spawningPipePtr, SessionInfo sessionInfo)
{
	const int pipeIndex = spawningPipePtr->GetWarpPipeIndex();
	EnterNewLevel(spawningPipePtr->GetWarpLevelIndex(), sessionInfo);
	m_CurrentLevelPtr->WarpPlayerToPipe(pipeIndex);
}

void GameState::EnterNewLevel(int levelIndex, SessionInfo sessionInfo, Pipe* spawningPipePtr)
{
	delete m_CurrentLevelPtr;
	m_CurrentLevelPtr = new Level(m_StateManagerPtr->GetGamePtr(), this, LevelProperties::Get(levelIndex), sessionInfo, spawningPipePtr);
}

bool GameState::ShowingSessionInfo() const
{
	return m_ShowingSessionInfo;
}
