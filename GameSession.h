#pragma once

class Level;

class GameSession
{
public:
	GameSession(const GameSession&) = delete;
	GameSession& operator=(const GameSession&) = delete;

	static void WriteSessionInfoToFile(bool startInfo, Level* levelPtr);
	static void ReadSessionInfoFromFile();
	
	static void NextSession();
	static void PreviousSession();

	static void PaintCurrentSessionInfo();

	static void Reset();

private:
	GameSession();
	virtual ~GameSession();

	/* NOTE: A reverseSessionIndex of 0 returns the most recent session info, 1 -> the second most recent, etc. */
	static std::string GetReadableSessionInfo(int reverseSessionIndex);
	static int GetNumberOfSessions();

	static std::string GetSessionInfoContent(std::string sessionString);
	static std::string GetTagContentIfAvailable(std::string sessionString, std::string tagString, std::string formattedString = "");

	static int m_CurrentSessionIndex;
	static int m_TotalSessionsWithInfo;
	static std::string m_CurrentSessionInfo;
	static std::string m_AllSessionInfo;
};
