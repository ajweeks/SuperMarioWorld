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
#include "StateManager.h"
#include "GameState.h"
#include "LevelInfo.h"

// Static initializations
Font* Game::Font12Ptr = nullptr;
Font* Game::Font9Ptr = nullptr;
Font* Game::Font6Ptr = nullptr;

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
#if SMW_DEFAULT_MUTE
	SoundManager::SetMuted(true);
#endif

	// TODO: Add mario fonts
	Game::Font12Ptr = new Font(String("consolas"), 12);
	Game::Font9Ptr = new Font(String("consolas"), 9);
	Game::Font6Ptr = new Font(String("consolas"), 6);

	LevelInfo::Initialize();

	matIdentity = MATRIX3X2::CreateScalingMatrix(WINDOW_SCALE);

#if SMW_DEBUG_ZOOM_OUT
	matIdentity = MATRIX3X2::CreateScalingMatrix(0.65) * MATRIX3X2::CreateTranslationMatrix(150, 0);
#endif

	m_StateManagerPtr = new StateManager(this);

	Reset();
}

void Game::Reset()
{
	GameSession::Reset();

	m_ShowingSessionInfo = false;
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
	delete Font12Ptr;
	delete Font9Ptr;
	delete Font6Ptr;

	LevelData::UnloadAllLevels();
	SpriteSheetManager::Unload();

	SoundManager::UnloadSoundsAndSongs();

	delete m_StateManagerPtr;
}

void Game::GameTick(double deltaTime)
{	
	if (GAME_ENGINE->IsKeyboardKeyPressed('I'))
	{
		m_ShowingSessionInfo = !m_ShowingSessionInfo;

		if (m_StateManagerPtr->CurrentState()->GetType() == STATE_TYPE::GAME)
		{
			((GameState*)m_StateManagerPtr->CurrentState())->SetPaused(m_ShowingSessionInfo, true);
		}
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
	}

	if (GAME_ENGINE->IsKeyboardKeyPressed('M'))
	{
		SoundManager::ToggleMuted();
	}

	m_StateManagerPtr->Tick(deltaTime);
}

void Game::GamePaint()
{
	GAME_ENGINE->SetViewMatrix(matIdentity);

	m_StateManagerPtr->Paint();

	if (m_ShowingSessionInfo)
	{
		MATRIX3X2 matPrevView = GAME_ENGINE->GetViewMatrix();
		GAME_ENGINE->SetViewMatrix(matIdentity);
		GameSession::PaintCurrentSessionInfo();
		GAME_ENGINE->SetViewMatrix(matPrevView);
	}
}

bool Game::ShowingSessionInfo()
{
	return m_ShowingSessionInfo;
}
