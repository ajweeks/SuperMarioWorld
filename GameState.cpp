#include "stdafx.h"

#include "GameState.h"
#include "Game.h"
#include "GameSession.h"
#include "StateManager.h"

GameState::GameState(StateManager* stateManagerPtr) :
	BaseState(stateManagerPtr, STATE_TYPE::GAME)
{

	GameSession::ReadSessionInfoFromFile();

	m_LevelPtr = new Level(m_StateManagerPtr->GetGamePtr());
	ResetMembers();
	
	GameSession::RecordStartSessionInfo(m_LevelPtr);
}

GameState::~GameState()
{
	GameSession::WriteSessionInfoToFile(m_LevelPtr);
	delete m_LevelPtr;
}

void GameState::Tick(double deltaTime)
{
	if (GAME_ENGINE->IsKeyboardKeyPressed('R'))
	{
		Reset();
	}

	if (GAME_ENGINE->IsKeyboardKeyPressed('P'))
	{
		// TODO: Find out why the game is so laggy when physics debug overlay is rendering
		m_RenderDebugOverlay = !m_RenderDebugOverlay;
		GAME_ENGINE->EnablePhysicsDebugRendering(m_RenderDebugOverlay);
	}

	m_LevelPtr->Tick(deltaTime);
}

void GameState::Paint()
{
	m_LevelPtr->Paint();
}

void GameState::Reset()
{
	m_LevelPtr->Reset();

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
	m_LevelPtr->SetPaused(paused);
}
