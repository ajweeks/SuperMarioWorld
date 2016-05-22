#include "stdafx.h"

#include "GameState.h"
#include "Game.h"
#include "GameSession.h"
#include "StateManager.h"
#include "LevelInfo.h"
#include "SpriteSheetManager.h"
#include "SoundManager.h"
#include "SessionInfo.h"
#include "Pipe.h"

GameState::GameState(StateManager* stateManagerPtr) :
	BaseState(stateManagerPtr, STATE_TYPE::GAME)
{
	GameSession::ReadSessionInfoFromFile();

	int levelIndex = 0; // Start the player in level 0
	m_LevelOverworldPtr = new Level(m_StateManagerPtr->GetGamePtr(), this, LevelInfo::levelInfoArr[levelIndex]);
	m_CurrentLevelPtr = m_LevelOverworldPtr;

	ResetMembers();
	
	GameSession::RecordStartSessionInfo(m_CurrentLevelPtr);
}

GameState::~GameState()
{
	GameSession::WriteSessionInfoToFile(m_CurrentLevelPtr);
	delete m_LevelOverworldPtr;

	if (m_LevelUndergroundPtr != nullptr)
	{
		delete m_LevelUndergroundPtr;
	}
}

void GameState::Reset()
{
	m_CurrentLevelPtr->Reset();

	m_StateManagerPtr->GetGamePtr()->Reset();

	ResetMembers();
}

void GameState::ResetMembers()
{
	m_RenderDebugOverlay = false;
	GAME_ENGINE->EnablePhysicsDebugRendering(m_RenderDebugOverlay);
}

void GameState::Tick(double deltaTime)
{
	if (m_InFrameByFrameMode && m_CurrentLevelPtr->IsPaused() == false)
	{
		// We've advanced by one frame, now we need to pause again
		m_CurrentLevelPtr->SetPaused(true, true);
	}
		
	if (GAME_ENGINE->IsKeyboardKeyPressed(VK_OEM_PERIOD))
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
	else if (GAME_ENGINE->IsKeyboardKeyPressed(VK_SPACE) && m_InFrameByFrameMode)
	{
		m_InFrameByFrameMode = false;
		m_CurrentLevelPtr->SetPaused(false, true);
	}

	if (GAME_ENGINE->IsKeyboardKeyPressed('R'))
	{
		Reset();
	}

	if (GAME_ENGINE->IsKeyboardKeyPressed('P'))
	{
		m_RenderDebugOverlay = !m_RenderDebugOverlay;
		GAME_ENGINE->EnablePhysicsDebugRendering(m_RenderDebugOverlay);
	}

	m_CurrentLevelPtr->Tick(deltaTime);
}

void GameState::Paint()
{
	m_CurrentLevelPtr->Paint();
}

void GameState::SetPaused(bool paused, bool pauseSongs)
{
	m_CurrentLevelPtr->SetPaused(paused, pauseSongs);
}

void GameState::EnterUnderground(SessionInfo sessionInfo, Pipe* pipePtr)
{
	if (m_LevelUndergroundPtr == nullptr)
	{
		int warpLevelIndex = pipePtr->GetWarpLevelIndex();
		int warpPipeIndex = pipePtr->GetWarpPipeIndex();

		assert(warpLevelIndex != -1);

		m_LevelUndergroundPtr = new Level(m_StateManagerPtr->GetGamePtr(), this, LevelInfo::levelInfoArr[warpLevelIndex], sessionInfo);
		m_CurrentLevelPtr = m_LevelUndergroundPtr;

		if (warpPipeIndex != -1)
		{
			m_CurrentLevelPtr->WarpPlayerToPipe(warpPipeIndex);
		}

		m_CurrentLevelPtr->SetPaused(true, false);
	}
}

void GameState::LeaveUnderground(SessionInfo sessionInfo, Pipe* pipeEnteredPtr)
{
	m_CurrentLevelPtr = m_LevelOverworldPtr;

	if (m_LevelUndergroundPtr != nullptr)
	{
		int warpLevelIndex = pipeEnteredPtr->GetWarpLevelIndex();
		int warpPipeIndex = pipeEnteredPtr->GetWarpPipeIndex();

		if (warpPipeIndex != -1)
		{
			m_CurrentLevelPtr->WarpPlayerToPipe(warpPipeIndex);
		}

		delete m_LevelUndergroundPtr;
		m_LevelUndergroundPtr = nullptr;
	}
}
