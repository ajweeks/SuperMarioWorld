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
#include "Keybindings.h"

// Static initializations
Font* Game::Font12Ptr = nullptr;
Font* Game::Font9Ptr = nullptr;
Font* Game::Font6Ptr = nullptr;

const int Game::WIDTH = 256;
const int Game::HEIGHT = 224;

const int Game::WINDOW_SCALE = 3;

MATRIX3X2 Game::matIdentity;

bool Game::DEBUG_SHOWING_CAMERA_INFO = false;
bool Game::DEBUG_SHOWING_PLAYER_INFO = false;
bool Game::DEBUG_SHOWING_ENEMY_AI_INFO = false;
const bool Game::DEBUG_SKIP_MAIN_MENU = false;
const bool Game::DEBUG_ZOOM_OUT = false;
const bool Game::DEBUG_START_MUTED = false;

const int Game::DEBUG_TELEPORT_PLAYER_TO_YOSHI = 840;
const int Game::DEBUG_TELEPORT_PLAYER_TO_CHARGIN_CHUCK = 2000;
const int Game::DEBUG_TELEPORT_PLAYER_TO_MONTY_MOLES = 3000;
const int Game::DEBUG_TELEPORT_PLAYER_TO_PIPES = 3890;
const int Game::DEBUG_TELEPORT_PLAYER_TO_PIRANHA_PLANT = 4270;
const int Game::DEBUG_TELEPORT_PLAYER_TO_GOAL_GATE = 4700;
const int Game::DEBUG_TELEPORT_PLAYER_TO_POSITION = DEBUG_TELEPORT_PLAYER_TO_YOSHI;

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
	Keybindings::ReadBindingsFromFile();

	SpriteSheetManager::Load();

	SoundManager::InitialzeSoundsAndSongs();
	if (DEBUG_START_MUTED)
	{
		SoundManager::SetMuted(true);
	}

	Game::Font12Ptr = new Font(String("consolas"), 12);
	Game::Font9Ptr = new Font(String("consolas"), 9);
	Game::Font6Ptr = new Font(String("consolas"), 6);

	LevelInfo::Initialize();

	matIdentity = MATRIX3X2::CreateScalingMatrix(WINDOW_SCALE);

	if (DEBUG_ZOOM_OUT)
	{
		matIdentity = MATRIX3X2::CreateScalingMatrix(0.65) * MATRIX3X2::CreateTranslationMatrix(150, 0);
	}

	m_StateManagerPtr = new StateManager(this);

	Reset();
}

void Game::Reset()
{
	GameSession::Reset();
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
	if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::TOGGLE_MUTED))
	{
		SoundManager::ToggleMuted();
	}
	
	if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::DEBUG_TOGGLE_CAMERA_DEBUG_OVERLAY))
	{
		DEBUG_SHOWING_CAMERA_INFO = !DEBUG_SHOWING_CAMERA_INFO;
	}
	if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::DEBUG_TOGGLE_PLAYER_INFO))
	{
		DEBUG_SHOWING_PLAYER_INFO = !DEBUG_SHOWING_PLAYER_INFO;
	}
	if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::DEBUG_TOGGLE_ENEMY_AI_OVERLAY))
	{
		DEBUG_SHOWING_ENEMY_AI_INFO = !DEBUG_SHOWING_ENEMY_AI_INFO;
	}

	m_StateManagerPtr->Tick(deltaTime);
}

void Game::GamePaint()
{
	GAME_ENGINE->SetViewMatrix(matIdentity);

	m_StateManagerPtr->Paint();
}
