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

#define SMW_ENABLE_JUMP_TO true
#define SMW_JUMP_TO_POS_X 4500

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

	static Font *Font16Ptr;
	static Font *Font24Ptr;

	// How many pixels we have to work with, not how large the final window will be necessarily
	static const int WIDTH = 256;
	static const int HEIGHT = 224;

	static const int WINDOW_SCALE = 2;
	// NOTE: Use this matrix as the base for all other matricies,
	// the hud should be drawn with just this as the view matrix
	// This matrix will be updated any time the screen size changes
	static MATRIX3X2 matIdentity;

private:
	void WriteSessionInfoToFile();
	std::string ReadSessionInfoFromFile();

	SYSTEMTIME m_StartTime;
	bool m_ShowingSessionInfo = false; // 'i' toggles info overlay
	std::string m_AllSessionInfo;
	int m_TotalSessionsWithInfo;
	int m_CurrentSessionInfoShowingIndex = 0;
	std::string m_CurrentSessionInfo;

	/* NOTE: A reverseSessionIndex of 0 returns the most recent session info, 1 -> the second most recent, etc. */
	std::string GetReadableSessionInfo(int reverseSessionIndex);
	int GetNumberOfSessions(std::string allSessionInfo);

	bool m_RenderDebugOverlay;

	Level *m_LevelPtr = nullptr;
};
