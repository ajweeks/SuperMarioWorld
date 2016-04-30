#pragma once

class Level;

struct SessionInfo
{
	std::string m_Date = "";
	std::string m_Time = "";
	std::string m_PlayerPowerupState = "";
	int m_PlayerLives = -1;
	int m_PlayerScore = -1;
	int m_TimeRemaining = -1;
	int m_CoinsCollected = -1;
	int m_CheckpointCleared = -1;
	int m_PlayerRidingYoshi = -1;
	int m_AllDragonCoinsCollected = -1;
};

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

	/* NOTE: A reverseSessionIndex of 0 returns the most recent session info, 1 -> the second most recent, etc. */
	static int GetNumberOfSessions();

	static void RecordSessionInfo(SessionInfo &sessionInfo, Level* levelPtr);

	static SessionInfoPair GetSessionInfo(int reverseSessionIndex);
	static SessionInfo GetSessionInfo(std::string sessionString);
	static std::string GetSessionInfoMarkedUp(SessionInfo sessionInfo);

	static std::string GetTimeDuration(std::string startTimeStr, std::string endTimeStr);

	static void PaintInfoString(std::string preString, int value1, int value2, int x, int& y);
	static void PaintInfoString(std::string preString, std::string value1, std::string value2, int x, int& y);
	static void PaintInfoString(std::string preString, bool value1, bool value2, int x, int& y);

	static int m_CurrentSessionIndex;
	static int m_TotalSessionsWithInfo;
	static SessionInfoPair m_CurrentSessionInfo;
	static std::string m_AllSessionsInfoString;
};
