#include "stdafx.h"

#include "GameSession.h"
#include "Enumerations.h"
#include "Level.h"
#include "Player.h"
#include "Game.h"
#include "CountdownTimer.h"
#include "FileIO.h"

int GameSession::m_CurrentSessionIndex = 0;
int GameSession::m_TotalSessionsWithInfo = 0;
SessionInfoPair GameSession::m_CurrentSessionInfo = {};
std::string GameSession::m_AllSessionsInfoString = "";

COLOR GameSession::OFF_WHITE = COLOR(245, 245, 250);
COLOR GameSession::GREEN = COLOR(16, 181, 16);
COLOR GameSession::RED = COLOR(193, 36, 36);

void GameSession::RecordStartSessionInfo(Level* levelPtr)
{
	RecordSessionInfo(m_CurrentSessionInfo.sessionInfoStart, levelPtr);
}

void GameSession::RecordSessionInfo(SessionInfo &sessionInfo, Level* levelPtr)
{
	SYSTEMTIME currentTime;
	GetLocalTime(&currentTime);

	std::stringstream stringStream;
	sessionInfo = {};

	stringStream <<
		std::to_string(currentTime.wYear) << ":" <<
		std::setw(2) << std::setfill('0') << std::to_string(currentTime.wMonth) << ":" <<
		std::setw(2) << std::setfill('0') << std::to_string(currentTime.wDay);
	sessionInfo.m_Date = stringStream.str();
	stringStream.str(std::string());
	stringStream.clear();

	stringStream << 
		std::setw(2) << std::setfill('0') << std::to_string(currentTime.wHour) << ":" <<
		std::setw(2) << std::setfill('0') << std::to_string(currentTime.wMinute) << ":" <<
		std::setw(2) << std::setfill('0') << std::to_string(currentTime.wSecond);
	sessionInfo.m_Time = stringStream.str();
	stringStream.str(std::string());
	stringStream.clear();

	sessionInfo.m_PlayerLives = levelPtr->GetPlayer()->GetLives();
	sessionInfo.m_PlayerScore = levelPtr->GetPlayer()->GetScore();
	sessionInfo.m_CheckpointCleared = levelPtr->IsCheckpointCleared();
	sessionInfo.m_PlayerRidingYoshi = levelPtr->GetPlayer()->IsRidingYoshi();
	sessionInfo.m_PlayerPowerupState = Player::PowerupStateToString(levelPtr->GetPlayer()->GetPowerupState());
	sessionInfo.m_TimeRemaining = levelPtr->GetTimeRemaining();
	sessionInfo.m_CoinsCollected = levelPtr->GetPlayer()->GetCoinsCollected();
	sessionInfo.m_AllDragonCoinsCollected = levelPtr->AllDragonCoinsCollected();
}

void GameSession::WriteSessionInfoToFile(Level* levelPtr)
{
	RecordSessionInfo(m_CurrentSessionInfo.sessionInfoEnd, levelPtr);

	std::ofstream fileOutStream;

	fileOutStream.open("Resources/GameSessions.txt", std::fstream::app);

	if (fileOutStream.fail() == false)
	{
		fileOutStream << "<Session>" << std::endl;

		fileOutStream << "\t<Start>" << std::endl;
		fileOutStream << GetSessionInfoMarkedUp(m_CurrentSessionInfo.sessionInfoStart);
		fileOutStream << "\t</Start>" << std::endl;

		fileOutStream << "\t<End>" << std::endl;
		fileOutStream << GetSessionInfoMarkedUp(m_CurrentSessionInfo.sessionInfoEnd);
		fileOutStream << "\t</End>" << std::endl;

		fileOutStream << "</Session>" << std::endl;
	}

	fileOutStream.close();
}

std::string GameSession::GetSessionInfoMarkedUp(SessionInfo sessionInfo)
{
	std::stringstream stringStream;
	stringStream << std::setfill('0');
	stringStream << "\t\t<Date>" << sessionInfo.m_Date << "</Date>" << std::endl;
	stringStream << "\t\t<Time>" << sessionInfo.m_Time << "</Time>" << std::endl;
	stringStream << "\t\t<PlayerLives>" << std::setw(2) << sessionInfo.m_PlayerLives << "</PlayerLives>" << std::endl;
	stringStream << "\t\t<Score>" << sessionInfo.m_PlayerScore << "</Score>" << std::endl;
	stringStream << "\t\t<CheckpointCleared>" << sessionInfo.m_CheckpointCleared << "</CheckpointCleared>" << std::endl;
	stringStream << "\t\t<PlayerRidingYoshi>" << sessionInfo.m_PlayerRidingYoshi << "</PlayerRidingYoshi>" << std::endl;
	stringStream << "\t\t<PlayerPowerupState>" << sessionInfo.m_PlayerPowerupState << "</PlayerPowerupState>" << std::endl;
	stringStream << "\t\t<TimeRemaining>" << sessionInfo.m_TimeRemaining << "</TimeRemaining>" << std::endl;
	stringStream << "\t\t<CoinsCollected>" << sessionInfo.m_CoinsCollected << "</CoinsCollected>" << std::endl;
	stringStream << "\t\t<AllDragonCoinsCollected>" << sessionInfo.m_AllDragonCoinsCollected << "</AllDragonCoinsCollected>" << std::endl;

	return stringStream.str();
}

void GameSession::ReadSessionInfoFromFile()
{
	std::ifstream fileInStream;
	std::stringstream stringStream;

	fileInStream.open("Resources/GameSessions.txt");

	std::string line;
	while (fileInStream.eof() == false)
	{
		std::getline(fileInStream, line);
		stringStream << line << "\n";
	}

	fileInStream.close();

	m_AllSessionsInfoString = stringStream.str();
	m_AllSessionsInfoString.erase(std::remove_if(m_AllSessionsInfoString.begin(), m_AllSessionsInfoString.end(), isspace), m_AllSessionsInfoString.end());
	m_TotalSessionsWithInfo = GetNumberOfSessions();
	m_CurrentSessionInfo = GetSessionInfo(m_CurrentSessionIndex);
}

int GameSession::GetNumberOfSessions()
{
	int numberOfSessions = 0;
	int currentIndex = -1;
	do
	{
		currentIndex = m_AllSessionsInfoString.find("<Session>", currentIndex + 1);
		numberOfSessions++;
	} while (currentIndex != std::string::npos);

	numberOfSessions--;

	return numberOfSessions;
}

SessionInfoPair GameSession::GetSessionInfo(int sessionIndex)
{
	int sessionsFound = 0;
	int currentIndex = -1;
	do
	{
		currentIndex = m_AllSessionsInfoString.find("<Session>", currentIndex + 1);
		sessionsFound++;
	} while (currentIndex != std::string::npos && m_TotalSessionsWithInfo - sessionsFound > sessionIndex);

	int sessionTagStart = currentIndex;
	int sessionTagEnd = m_AllSessionsInfoString.find("</Session>", sessionTagStart);

	if (sessionTagStart != std::string::npos && sessionTagEnd != std::string::npos)
	{
		std::string currentSessionRaw;
		currentSessionRaw = m_AllSessionsInfoString.substr(sessionTagStart, sessionTagEnd - sessionTagStart);

		SessionInfo currentSessionInfoStart;
		int startTag = currentSessionRaw.find("<Start>") + std::string("<Start>").length();
		int startTagEnd = currentSessionRaw.find("</Start>") + std::string("</Start>").length();
		if (startTag != std::string::npos && startTagEnd != std::string::npos)
		{
			std::string startInfo = currentSessionRaw.substr(startTag, startTagEnd - startTag);
			currentSessionInfoStart = GetSessionInfo(startInfo);
		}

		SessionInfo currentSessionInfoEnd;
		int endTag = currentSessionRaw.find("<End>") + std::string("<End>").length();
		int endTagEnd = currentSessionRaw.find("</End>") + std::string("</End>").length();
		if (endTag != std::string::npos && endTagEnd != std::string::npos)
		{
			std::string endInfo = currentSessionRaw.substr(endTag, endTagEnd - endTag);
			currentSessionInfoEnd = GetSessionInfo(endInfo);
		}

		SessionInfoPair sessionInfoPair;
		sessionInfoPair.sessionInfoStart = currentSessionInfoStart;
		sessionInfoPair.sessionInfoEnd = currentSessionInfoEnd;
		sessionInfoPair.m_SessionIndex = sessionIndex;
		return sessionInfoPair;
	}
	return {};
}

SessionInfo GameSession::GetSessionInfo(std::string sessionString)
{
	SessionInfo sessionInfo = {};
	
	sessionInfo.m_Date = FileIO::GetTagContent(sessionString, "Date");
	sessionInfo.m_Time = FileIO::GetTagContent(sessionString, "Time");

	std::string playerPowerupStateStr = FileIO::GetTagContent(sessionString, "PlayerPowerupState");
	if (playerPowerupStateStr.length() > 0) sessionInfo.m_PlayerPowerupState = playerPowerupStateStr;

	std::string playerLivesStr = FileIO::GetTagContent(sessionString, "PlayerLives");
	if (playerLivesStr.length() > 0) sessionInfo.m_PlayerLives = stoi(playerLivesStr);

	std::string playerScoreStr = FileIO::GetTagContent(sessionString, "Score");
	if (playerScoreStr.length() > 0) sessionInfo.m_PlayerScore = stoi(playerScoreStr);

	std::string timeRemainingStr = FileIO::GetTagContent(sessionString, "TimeRemaining");
	if (timeRemainingStr.length() > 0) sessionInfo.m_TimeRemaining = stoi(timeRemainingStr);

	std::string coinsCollectedStr = FileIO::GetTagContent(sessionString, "CoinsCollected");
	if(coinsCollectedStr.length() > 0) sessionInfo.m_CoinsCollected = stoi(coinsCollectedStr);

	std::string checkpointClearedStr = FileIO::GetTagContent(sessionString, "CheckpointCleared");
	if (checkpointClearedStr.length() > 0) sessionInfo.m_CheckpointCleared = FileIO::StringToBool(checkpointClearedStr);

	std::string playerRidingYoshiStr = FileIO::GetTagContent(sessionString, "PlayerRidingYoshi");
	if (playerRidingYoshiStr.length() > 0) sessionInfo.m_PlayerRidingYoshi = FileIO::StringToBool(playerRidingYoshiStr);

	std::string allDragonCoinsCollectedStr = FileIO::GetTagContent(sessionString, "AllDragonCoinsCollected");
	if (allDragonCoinsCollectedStr.length() > 0) sessionInfo.m_AllDragonCoinsCollected = FileIO::StringToBool(allDragonCoinsCollectedStr);

	return sessionInfo;
}

void GameSession::PaintCurrentSessionInfo()
{
	// BACKGROUND ROWS
	int rows = 11;
	int yOffset = 34;
	GAME_ENGINE->SetColor(COLOR(5, 5, 5, 185));
	GAME_ENGINE->FillRect(0, 0, Game::WIDTH, yOffset);
	if (yOffset + rows * LINE_HEIGHT < Game::HEIGHT)
	{
		GAME_ENGINE->FillRect(0, yOffset + rows * LINE_HEIGHT, Game::WIDTH, Game::HEIGHT);
	}

	for (int i = 0; i < rows; ++i)
	{
		if (i% 2 == 0) GAME_ENGINE->SetColor(COLOR(10, 10, 10, 185));
		else GAME_ENGINE->SetColor(COLOR(25, 25, 30, 185));

		GAME_ENGINE->FillRect(0, i * LINE_HEIGHT + yOffset, Game::WIDTH, (i + 1) * LINE_HEIGHT + yOffset);
	}

	int x = 12;
	int y = 10;

	// SESSION INDEX
	GAME_ENGINE->SetColor(OFF_WHITE);
	GAME_ENGINE->SetFont(Game::Font6Ptr);
	GAME_ENGINE->DrawString(String("<-PgUp|PgDn->"), Game::WIDTH - 48, y - 6);

	GAME_ENGINE->SetFont(Game::Font9Ptr);
	GAME_ENGINE->DrawString(String("Current session: ") + String(m_TotalSessionsWithInfo - m_CurrentSessionInfo.m_SessionIndex) +
		String("/") + String(m_TotalSessionsWithInfo), x, y);
	y += 16;

	// HEADINGS
	GAME_ENGINE->SetFont(Game::Font12Ptr);
	GAME_ENGINE->DrawString(String("Start"), x + 15, y);
	GAME_ENGINE->DrawString(String("End"), x + 170, y);

	y += 20;
	GAME_ENGINE->DrawLine(10, y, Game::WIDTH - 20, y);

	// MAIN INFO
	GAME_ENGINE->SetFont(Game::Font9Ptr);
	m_CurrentSessionInfo;
	
	PaintInfoString("Date: ",
		m_CurrentSessionInfo.sessionInfoStart.m_Date,
		m_CurrentSessionInfo.sessionInfoEnd.m_Date,
		x, y);

	PaintInfoString("Time: ",
		m_CurrentSessionInfo.sessionInfoStart.m_Time,
		m_CurrentSessionInfo.sessionInfoEnd.m_Time,
		x, y);
	std::string timeDurationStr = GetTimeDuration(m_CurrentSessionInfo.sessionInfoStart.m_Time, m_CurrentSessionInfo.sessionInfoEnd.m_Time);
	GAME_ENGINE->DrawString(String("(") + String(timeDurationStr.c_str())+ String(")"), x + COL_WIDTH, y);
	y += LINE_HEIGHT;

	PaintInfoString("Player lives: ",
		m_CurrentSessionInfo.sessionInfoStart.m_PlayerLives,
		m_CurrentSessionInfo.sessionInfoEnd.m_PlayerLives,
		x, y);

	PaintInfoString("Score: ",
		m_CurrentSessionInfo.sessionInfoStart.m_PlayerScore,
		m_CurrentSessionInfo.sessionInfoEnd.m_PlayerScore,
		x, y);

	PaintInfoString("Coins: ",
		m_CurrentSessionInfo.sessionInfoStart.m_CoinsCollected,
		m_CurrentSessionInfo.sessionInfoEnd.m_CoinsCollected,
		x, y);

	PaintInfoString("Player powerup state: ",
		m_CurrentSessionInfo.sessionInfoStart.m_PlayerPowerupState,
		m_CurrentSessionInfo.sessionInfoEnd.m_PlayerPowerupState,
		x, y);

	PaintInfoString("Time remaining: ",
		m_CurrentSessionInfo.sessionInfoStart.m_TimeRemaining,
		m_CurrentSessionInfo.sessionInfoEnd.m_TimeRemaining,
		x, y);

	PaintInfoString("Checkpoint cleared: ",
		m_CurrentSessionInfo.sessionInfoStart.m_CheckpointCleared,
		m_CurrentSessionInfo.sessionInfoEnd.m_CheckpointCleared,
		x, y);

	PaintInfoString("Riding yoshi: ",
		m_CurrentSessionInfo.sessionInfoStart.m_PlayerRidingYoshi,
		m_CurrentSessionInfo.sessionInfoEnd.m_PlayerRidingYoshi,
		x, y);

	PaintInfoString("All dragon coins collected: ",
		m_CurrentSessionInfo.sessionInfoStart.m_AllDragonCoinsCollected,
		m_CurrentSessionInfo.sessionInfoEnd.m_AllDragonCoinsCollected,
		x, y);
}

void GameSession::PaintInfoString(std::string preString, int value1, int value2, int x, int& y)
{
	if (value1 == -1 || value2 == -1) return;

	GAME_ENGINE->SetColor(OFF_WHITE);
	GAME_ENGINE->DrawString(String(preString.c_str()) + String(value1), x, y);
	if (value1 > value2)
	{
		GAME_ENGINE->SetColor(RED);
		GAME_ENGINE->DrawString(String(value2), x + COL_WIDTH, y);
	}
	else if (value1 < value2)
	{
		GAME_ENGINE->SetColor(GREEN);
		GAME_ENGINE->DrawString(String(value2), x + COL_WIDTH, y);
	}
	y += LINE_HEIGHT;
}

void GameSession::PaintInfoString(std::string preString, std::string value1, std::string value2, int x, int& y)
{
	if (!value1.compare("") || !value2.compare("")) return;

	GAME_ENGINE->SetColor(OFF_WHITE);
	GAME_ENGINE->DrawString(String(preString.c_str()) + String(value1.c_str()), x, y);
	if (value1.compare(value2) > 0)
	{
		GAME_ENGINE->SetColor(RED);
		GAME_ENGINE->DrawString(String(value2.c_str()), x + COL_WIDTH, y);
	}
	else if (value1.compare(value2) < 0)
	{
		GAME_ENGINE->SetColor(GREEN);
		GAME_ENGINE->DrawString(String(value2.c_str()), x + COL_WIDTH, y);
	}
	y += LINE_HEIGHT;
}

void GameSession::PaintInfoString(std::string preString, bool value1, bool value2, int x, int& y)
{
	if (int(value1) == -1 || int(value2) == -1) return;

	GAME_ENGINE->SetColor(OFF_WHITE);
	GAME_ENGINE->DrawString(String(preString.c_str()) + String(FileIO::BoolToString(value1).c_str()), x, y);
	if (value1 && !value2)
	{
		GAME_ENGINE->SetColor(RED);
		GAME_ENGINE->DrawString(String(FileIO::BoolToString(value2).c_str()), x + COL_WIDTH, y);
	}
	else if (value2 && !value1)
	{
		GAME_ENGINE->SetColor(GREEN);
		GAME_ENGINE->DrawString(String(FileIO::BoolToString(value2).c_str()), x + COL_WIDTH, y);
	}
	y += LINE_HEIGHT;
}

// This function expects both input strings to be formatted as such: "HH:MM:SS"
std::string GameSession::GetTimeDuration(std::string startTimeStr, std::string endTimeStr)
{
	std::stringstream result;

	int dHours = stoi(endTimeStr.substr(0, 2)) - stoi(startTimeStr.substr(0, 2));
	int dMinutes = stoi(endTimeStr.substr(3, 2)) - stoi(startTimeStr.substr(3, 2));
	int dSeconds = stoi(endTimeStr.substr(6, 2)) - stoi(startTimeStr.substr(6, 2));
	
	if (dSeconds < 0)
	{
		dSeconds += 60;
		dMinutes -= 1;
	}
	if (dMinutes < 0)
	{
		dMinutes += 60;
		dHours -= 1;
	}

	if (dHours != 0) result << std::to_string(dHours) << "h, ";

	if (dMinutes != 0) result << std::to_string(dMinutes) << "m, ";

	result << std::to_string(dSeconds) << "s";

	return result.str();
}

void GameSession::ShowNextSession()
{
	if (m_CurrentSessionIndex + 1 < m_TotalSessionsWithInfo)
	{
		m_CurrentSessionIndex++;
		m_CurrentSessionInfo = GetSessionInfo(m_CurrentSessionIndex);
	}
}

void GameSession::ShowPreviousSession()
{
	if (m_CurrentSessionIndex - 1 >= 0)
	{
		m_CurrentSessionIndex--;
		m_CurrentSessionInfo = GetSessionInfo(m_CurrentSessionIndex);
	}
}

void GameSession::Reset()
{
	m_CurrentSessionIndex = 0;
	m_CurrentSessionInfo = GameSession::GetSessionInfo(m_CurrentSessionIndex);
}

GameSession::GameSession()
{}

GameSession::~GameSession()
{}
