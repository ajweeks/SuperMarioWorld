#pragma once

#include "SessionInfo.h"

class Level;

struct SessionInfoPair
{
	int m_SessionIndex;

	SessionInfo sessionInfoStart;
	SessionInfo sessionInfoEnd;
};

class GameSession
{
public:

	GameSession(const GameSession&) = delete;
	GameSession& operator=(const GameSession&) = delete;

	static void RecordStartSessionInfo(Level* levelPtr);
	static void RecordSessionInfo(SessionInfo &sessionInfo, Level* levelPtr);
	static void WriteSessionInfoToFile(Level* levelPtr);
	static void ReadSessionInfoFromFile();

	static void ShowNextSession();
	static void ShowPreviousSession();

	static void PaintCurrentSessionInfo();

	static void Reset();

private:
	GameSession();
	virtual ~GameSession();

	static COLOR OFF_WHITE;
	static COLOR GREEN;
	static COLOR RED;
	
	static const int LINE_HEIGHT = 11;
	static const int COL_WIDTH = 165;

	static int GetNumberOfSessions();

	static SessionInfoPair GetSessionInfo();
	static SessionInfo GetSessionInfo(std::string sessionString);
	static std::string GetSessionInfoMarkedUp(SessionInfo sessionInfo);

	static std::string GetTimeDuration(std::string startTimeStr, std::string endTimeStr);

	static void PaintInfoString(std::string preString, std::string value1, std::string value2, int& x, int& y, bool positive);
	static void PaintInfoString(std::string preString, std::string value1, std::string value2, int& x, int& y);
	static void PaintInfoString(std::string preString, int value1, int value2, int& x, int& y);
	static void PaintInfoString(std::string preString, bool value1, bool value2, int& x, int& y);

	static int m_CurrentSessionIndexShowing;
	static SessionInfoPair m_CurrentSessionInfoShowing;

	// This should only be modified at the start and end of a game!
	static SessionInfoPair m_CurrentSessionInfoRecording;
	
	static int m_TotalSessionsWithInfo;
	static std::string m_AllSessionsInfoString;
};
