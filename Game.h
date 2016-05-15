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

#define SMW_SKIP_MAIN_MENU true
#define SMW_DISPLAY_GENERAL_DEBUG_INFO true
#define SMW_DISPLAY_CAMERA_DEBUG_INFO false
#define SMW_DEBUG_ZOOM_OUT false
#define SMW_DISPLAY_AI_DEBUG_INFO false

#define SMW_ENABLE_JUMP_TO
#define SMW_JUMP_TO_POS_X 840 // Yoshi
//#define SMW_JUMP_TO_POS_X 2000 // Chargin Chuck
//#define SMW_JUMP_TO_POS_X 3000 // Monty Moles
//#define SMW_JUMP_TO_POS_X 4270 // Piranha Plant
//#define SMW_JUMP_TO_POS_X 4700 // Goal Gate

#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))

#define CLAMP(value, minimum, maximum) (min(maximum, max(value, minimum)))
//#define CLAMP(value, minimum, maximum) (value < minimum ? minimum : value > maximum ? maximum : value)

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

	bool ShowingSessionInfo();

	void Reset();
	
	static Font *Font12Ptr;
	static Font *Font9Ptr;
	static Font *Font6Ptr;

	// NOTE: How many pixels we have to work with, not how large the final window will be necessarily
	static const int WIDTH;
	static const int HEIGHT;

	static const int WINDOW_SCALE = 3;
	// NOTE: Use this matrix as the base for all other matricies,
	// the hud should be drawn with just this as the view matrix
	// This matrix will be updated any time the screen size changes
	static MATRIX3X2 matIdentity;

private:

	bool m_ShowingSessionInfo = false; // 'i' toggles info overlay

	// This is set to the level's paused field when the window is defocused
	// so we can set paused back to what it was
	bool m_WasMuted = false;

	StateManager* m_StateManagerPtr = nullptr;
};
