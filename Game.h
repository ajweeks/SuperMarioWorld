//-----------------------------------------------------------------
// Game Engine Object
// C++ Header - Game.h - version v2_18 feb 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Student data
// Name: Weeks, Andrew
// Group: 1DAE06
//-----------------------------------------------------------------

#pragma once

#include "Resource.h"	
#include "AbstractGame.h"

#include "Level.h"

#define GAME_ENGINE (GameEngine::GetSingleton())

#define CLAMP(value, minimum, maximum) (min(maximum, max(value, minimum)))

class StateManager;

class Game : public AbstractGame
{
public:
	Game();
	virtual ~Game();

	// C++11 make the class non-copyable
	Game(const Game&) = delete;
	Game& operator=(const Game&) = delete;

	virtual void GameInitialize(GameSettings &gameSettingsRef);
	virtual void GameStart();
	virtual void GameEnd();
	virtual void GameTick(double deltaTime);
	virtual void GamePaint();

	virtual void GameSetSleeping(bool sleeping);

	void Reset();
	
	static Font *Font12Ptr;
	static Font *Font9Ptr;
	static Font *Font6Ptr;

	// How many pixels we have to work with, not how large the final window will be necessarily
	static const int WIDTH;
	static const int HEIGHT;

	static const int WINDOW_SCALE;

	// NOTE: Use this matrix as the base for all other matricies,
	// the hud should be drawn with just this as the view matrix
	// This matrix will be updated any time the screen size changes
	static MATRIX3X2 matIdentity;

	static bool DEBUG_SHOWING_CAMERA_INFO;
	static bool DEBUG_SHOWING_PLAYER_INFO;
	static bool DEBUG_SHOWING_ENEMY_AI_INFO;
	static const bool DEBUG_SKIP_MAIN_MENU;
	static const bool DEBUG_ZOOM_OUT;
	static const bool DEBUG_START_MUTED;
	
	static const int DEBUG_TELEPORT_PLAYER_TO_POSITION;

private:
	static const int DEBUG_TELEPORT_PLAYER_TO_YOSHI;
	static const int DEBUG_TELEPORT_PLAYER_TO_CHARGIN_CHUCK;
	static const int DEBUG_TELEPORT_PLAYER_TO_MONTY_MOLES;
	static const int DEBUG_TELEPORT_PLAYER_TO_PIPES;
	static const int DEBUG_TELEPORT_PLAYER_TO_PIRANHA_PLANT;
	static const int DEBUG_TELEPORT_PLAYER_TO_GOAL_GATE;

	// This is set to the level's paused field when the window is defocused
	// so we can set paused back to what it was
	bool m_WasMuted = false;

	StateManager* m_StateManagerPtr = nullptr;
};
