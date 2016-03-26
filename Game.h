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

	//static const int SCALE = 3;

	static Font *Font16Ptr;
	static Font *Font24Ptr;

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
	bool m_Paused = false;

	Level *m_LevelPtr = nullptr;
};
