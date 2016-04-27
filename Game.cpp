//-----------------------------------------------------------------
// Game Engine Object
// C++ Source - Game.cpp - version v2_18 feb 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

// this include must be the first include line of every cpp file (due to using precompiled header)
#include "stdafx.h"		

#include "Game.h"		
#include "SpriteSheetManager.h"
#include "LevelData.h"
#include "SoundManager.h"
#include "Player.h"
#include "GameSession.h"
#include "Enumerations.h"

Font* Game::Font12Ptr = nullptr;
Font* Game::Font9Ptr = nullptr;

const int Game::WIDTH = 256;
const int Game::HEIGHT = 224;

MATRIX3X2 Game::matIdentity;

Game::Game()
{
}

Game::~Game()
{
}

void Game::GameInitialize(GameSettings &gameSettingsRef)
{
	gameSettingsRef.SetWindowTitle(String("Super Mario World - Weeks, Andrew - 1DAE06"));

	gameSettingsRef.SetWindowWidth(WIDTH * WINDOW_SCALE);
	gameSettingsRef.SetWindowHeight(HEIGHT * WINDOW_SCALE);

	gameSettingsRef.EnableConsole(false);
	gameSettingsRef.EnableAntiAliasing(false);

	// NOTE: false prevents screen tearing
	gameSettingsRef.EnableVSync(false);

	// TODO: find out how to customize the icon in the menu bar
	//WORD wIcon;

	// NOTE: This line makes the sprites render without an ugly 
	// black line around the edges, BUT the original game seems
	// to have used some amount of blurring, so this does make it 
	// look a little too crisp.
	GAME_ENGINE->SetBitmapInterpolationModeNearestNeighbor();
}

void Game::GameStart()
{
	SpriteSheetManager::Load();

	SoundManager::InitialzeSoundsAndSongs();
	SoundManager::SetMuted(false);

	// TODO: Add mario fonts
	Game::Font12Ptr = new Font(String("consolas"), 12);
	Game::Font9Ptr = new Font(String("consolas"), 9);

	matIdentity = MATRIX3X2::CreateScalingMatrix(WINDOW_SCALE);

#if DEBUG_ZOOM_OUT
	matIdentity = MATRIX3X2::CreateScalingMatrix(0.65) * MATRIX3X2::CreateTranslationMatrix(150, 0);
#endif

	m_LevelPtr = new Level();

	GameSession::ReadSessionInfoFromFile();

	Reset();

	GameSession::RecordStartSessionInfo(m_LevelPtr);
}

void Game::Reset()
{
	m_LevelPtr->Reset();

	m_SecondsElapsed = 0.0;

	GameSession::Reset();

	m_ShowingSessionInfo = false;

	m_RenderDebugOverlay = false;
	GAME_ENGINE->EnablePhysicsDebugRendering(m_RenderDebugOverlay);
}

void Game::GameSetSleeping(bool sleeping)
{
	if (SoundManager::IsInitialized() == false) return;
	if (sleeping)
	{
		m_WasMuted = SoundManager::IsMuted();
		SoundManager::SetMuted(sleeping);
	}
	else
	{
		SoundManager::SetMuted(m_WasMuted);
	}
}

void Game::GameEnd()
{
	GameSession::WriteSessionInfoToFile(m_LevelPtr);

	delete m_LevelPtr;

	delete Font12Ptr;
	delete Font9Ptr;

	LevelData::Unload();
	SpriteSheetManager::Unload();

	SoundManager::UnloadSoundsAndSongs();
}

void Game::GameTick(double deltaTime)
{
	if (GAME_ENGINE->IsKeyboardKeyPressed('R'))
	{
		Reset();
	}
	if (GAME_ENGINE->IsKeyboardKeyPressed('I'))
	{
		m_ShowingSessionInfo = !m_ShowingSessionInfo;

		m_LevelPtr->SetPaused(m_ShowingSessionInfo);
	}

	if (m_ShowingSessionInfo)
	{
		if (GAME_ENGINE->IsKeyboardKeyPressed(VK_NEXT) ||
			(GAME_ENGINE->IsKeyboardKeyDown(VK_NEXT) && GAME_ENGINE->IsKeyboardKeyDown(VK_CONTROL))) // Page Down
		{
			GameSession::ShowNextSession();
		}
		if (GAME_ENGINE->IsKeyboardKeyPressed(VK_PRIOR) ||
			(GAME_ENGINE->IsKeyboardKeyDown(VK_PRIOR) && GAME_ENGINE->IsKeyboardKeyDown(VK_CONTROL))) // Page Up
		{
			GameSession::ShowPreviousSession();
		}

		return;
	}

	m_LevelPtr->Tick(deltaTime);

	if (GAME_ENGINE->IsKeyboardKeyPressed('P'))
	{
		// TODO: Find out why the game is so laggy when physics debug overlay is rendering
		m_RenderDebugOverlay = !m_RenderDebugOverlay;
		GAME_ENGINE->EnablePhysicsDebugRendering(m_RenderDebugOverlay);
	}
	if (GAME_ENGINE->IsKeyboardKeyPressed('M'))
	{
		SoundManager::ToggleMuted();
	}
}

void Game::GamePaint()
{
	m_LevelPtr->Paint();

	MATRIX3X2 matPrevious = GAME_ENGINE->GetViewMatrix();
	GAME_ENGINE->SetViewMatrix(matIdentity);
	if (m_ShowingSessionInfo)
	{
		GameSession::PaintCurrentSessionInfo();
	}

	GAME_ENGINE->SetViewMatrix(matPrevious);
}
