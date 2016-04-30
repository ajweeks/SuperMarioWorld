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

#define DEBUG_ZOOM_OUT false

#define SMW_ENABLE_JUMP_TO
#define SMW_JUMP_TO_POS_X 4700
//#define SMW_JUMP_TO_POS_X 2500

#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))

#define CLAMP(value, minimum, maximum) (min(maximum, max(value, minimum)))
//#define CLAMP(value, minimum, maximum) (value < minimum ? minimum : value > maximum ? maximum : value)

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
	void Reset();

	bool m_ShowingSessionInfo = false; // 'i' toggles info overlay

	bool m_RenderDebugOverlay;

	Level *m_LevelPtr = nullptr;
	// This is set to the level's paused field when the window is defocused
	// so we can set paused back to what it was
	bool m_WasMuted = false;

	double m_SecondsElapsed = 0.0;
};
