#include "stdafx.h"

#include "GameState.h"
#include "Game.h"
#include "GameSession.h"
#include "StateManager.h"
#include "LevelInfo.h"
#include "SpriteSheetManager.h"
#include "SoundManager.h"

GameState::GameState(StateManager* stateManagerPtr) :
	BaseState(stateManagerPtr, STATE_TYPE::GAME)
{
	GameSession::ReadSessionInfoFromFile();

	int index = 0;
	std::stringstream stream; 
	stream << std::setw(2) << std::setfill('0') << index;
	LevelInfo levelInfo { 
		index, String(("Resources/levels/" + stream.str() + "/level.svg").c_str()),
		SpriteSheetManager::levelOneBackgroundPtr,
		-1,
		SpriteSheetManager::levelForegroundPtrArr[index],
		SoundManager::SONG::OVERWORLD_BGM,
		SoundManager::SONG::OVERWORLD_BGM_FAST,
		SpriteSheetManager::levelForegroundPtrArr[index]->GetWidth(),
		SpriteSheetManager::levelForegroundPtrArr[index]->GetHeight() };

	m_LevelOverworldPtr = new Level(levelInfo, m_StateManagerPtr->GetGamePtr(), this);
	m_CurrentLevelPtr = m_LevelOverworldPtr;

	ResetMembers();
	
	GameSession::RecordStartSessionInfo(m_CurrentLevelPtr);
}

GameState::~GameState()
{
	GameSession::WriteSessionInfoToFile(m_CurrentLevelPtr);
	delete m_LevelOverworldPtr;
	delete m_LevelUndergroundPtr;
}

void GameState::Tick(double deltaTime)
{
	if (m_InFrameByFrameMode && m_CurrentLevelPtr->IsPaused() == false)
	{
		// We've advanced by one frame, now we need to pause again
		m_CurrentLevelPtr->SetPaused(true);
	}
		
	if (GAME_ENGINE->IsKeyboardKeyPressed(VK_OEM_PERIOD))
	{
		if (m_InFrameByFrameMode)
		{
			// Advance by one frame
			m_CurrentLevelPtr->SetPaused(false);
		}
		else
		{
			m_InFrameByFrameMode = true;
			m_CurrentLevelPtr->SetPaused(true);
		}
	}
	else if (GAME_ENGINE->IsKeyboardKeyPressed(VK_SPACE) && m_InFrameByFrameMode)
	{
		m_InFrameByFrameMode = false;
		m_CurrentLevelPtr->SetPaused(false);
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

void GameState::SetPaused(bool paused)
{
	m_CurrentLevelPtr->SetPaused(paused);
}

void GameState::EnterUnderground()
{
	if (m_LevelUndergroundPtr == nullptr)
	{
		int index = 1;
		std::stringstream stream;
		stream << std::setw(2) << std::setfill('0') << index;
		LevelInfo levelInfo{
			index, String(("Resources/levels/" + stream.str() + "/level.svg").c_str()),
			SpriteSheetManager::levelOneUndergroundBackgroundPtr,
			3,
			SpriteSheetManager::levelForegroundPtrArr[index],
			SoundManager::SONG::UNDERGROUND_BGM,
			SoundManager::SONG::UNDERGROUND_BGM_FAST,
			SpriteSheetManager::levelForegroundPtrArr[index]->GetWidth(),
			SpriteSheetManager::levelForegroundPtrArr[index]->GetHeight() };

		m_LevelUndergroundPtr = new Level(levelInfo, m_StateManagerPtr->GetGamePtr(), this);
		m_CurrentLevelPtr = m_LevelUndergroundPtr;
	}
}

void GameState::LeaveUnderground()
{
	if (m_LevelUndergroundPtr != nullptr)
	{
		m_CurrentLevelPtr = m_LevelOverworldPtr;
		delete m_LevelUndergroundPtr;
	}
}
