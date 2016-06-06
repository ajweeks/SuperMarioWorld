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
	
	static void CountDaysOfWeek();

	static void Tick(double deltaTime);
	static void Paint();

	static void Reset();

private:
	GameSession();
	virtual ~GameSession();

	static COLOR OFF_WHITE;
	static COLOR GREEN;
	static COLOR RED;
	
	static const int LINE_HEIGHT = 11;
	static const int COL_WIDTH = 165;

	static void ShowNextSession();
	static void ShowPreviousSession();

	static void ReadAllSessionData(); // Puts all available data into m_AllSessionInfoArr (fast, but takes some time if sessions file is large)
	static int GetNumberOfSessions();
	static SessionInfoPair FindSessionInfoPair(int sessionIndex); // Runs through the file string until it finds the nth session start tag (SLOW)
	static SessionInfo GetSessionInfo(const std::string& sessionString);
	static SessionInfoPair GetSessionInfoPair(const std::string& sessionString, int sessionIndex);

	static std::string GetSessionInfoMarkedUp(SessionInfo sessionInfo);
	static std::string GetTimeDuration(std::string startTimeStr, std::string endTimeStr);

	static void PaintInfoString(const std::string& preString, const std::string& value1, const std::string& value2, int& x, int& y, bool positive);
	static void PaintInfoString(const std::string& preString, const std::string& value1, const std::string& value2, int& x, int& y);
	static void PaintInfoString(const std::string& preString, int value1, int value2, int& x, int& y);
	static void PaintInfoString(const std::string& preString, bool value1, bool value2, int& x, int& y);

	static void PaintDaysOfWeekBarGraph(const std::vector<int>& daysOfWeekArr, int x, int y);
	static void PaintDaysOfWeekPieChart(const std::vector<int>& daysOfWeekArr, int x, int y);

	static std::vector<int> m_DaysOfWeekArr;
	static bool m_DaysOfWeekArrGenerated;
	static bool m_ShouldGenerateDaysOfWeek;

	static int m_CurrentSessionShowingIndex;

	// This should only be modified at the start and end of a game!
	static SessionInfoPair m_CurrentSessionInfoRecording;
	
	static std::string m_AllSessionsInfoString;
	static int m_NumberOfSessions;
	// This gets filled one element at a time, as the user requests them, until they press F2 to request all (for a nice graph)
	// The first element is the most recent, the last element is the oldest
	static std::vector<SessionInfoPair> m_AllSessionInfoArr; 
};
