#include "stdafx.h"

#include "GameSession.h"
#include "Enumerations.h"
#include "Level.h"
#include "Player.h"
#include "Game.h"

int GameSession::m_CurrentSessionIndex = 0;
int GameSession::m_TotalSessionsWithInfo = 0;
std::string GameSession::m_AllSessionInfo = "";
std::string GameSession::m_CurrentSessionInfo = "";

void GameSession::WriteSessionInfoToFile(bool writeStartInfo, Level* levelPtr)
{
	std::ofstream fileOutStream;

	fileOutStream.open("Resources/GameSessions.txt", std::fstream::app);

	if (fileOutStream.fail() == false)
	{
		int playerLives = levelPtr->GetPlayer()->GetLives();
		int playerScore = levelPtr->GetPlayer()->GetScore();
		bool checkpointCleared = levelPtr->IsCheckpointCleared();
		bool ridingYoshi = levelPtr->GetPlayer()->IsRidingYoshi();
		std::string playerPowerupState = levelPtr->GetPlayer()->GetPowerupStateString();
		int timeRemaining = levelPtr->GetTimeRemaining();
		int numberOfCoinsCollected = levelPtr->GetPlayer()->GetCoinsCollected();
		bool allDragonCoinsCollected = levelPtr->AllDragonCoinsCollected();

		SYSTEMTIME currentTime;
		GetSystemTime(&currentTime);

		if (writeStartInfo)
		{
			fileOutStream << "<Session>" << std::endl;
			fileOutStream << "\t<Start>" << std::endl;
		}
		else
		{
			fileOutStream << "\t<End>" << std::endl;
		}

		fileOutStream << std::setfill('0');
		fileOutStream << "\t\t<Date>";
		fileOutStream << currentTime.wYear << ":";
		fileOutStream << std::setw(2) << currentTime.wMonth << ":";
		fileOutStream << std::setw(2) << currentTime.wDay;
		fileOutStream << "</Date>" << std::endl;

		fileOutStream << "\t\t<Time>";
		fileOutStream << std::setw(2) << currentTime.wHour << ":";
		fileOutStream << std::setw(2) << currentTime.wMinute << ":";
		fileOutStream << std::setw(2) << currentTime.wSecond;
		fileOutStream << "</Time>" << std::endl;

		fileOutStream << "\t\t<PlayerLives>" << std::setw(2) << playerLives << "</PlayerLives>" << std::endl;
		fileOutStream << "\t\t<Score>" << playerScore << "</Score>" << std::endl;
		fileOutStream << "\t\t<CheckpointCleared>" << checkpointCleared << "</CheckpointCleared>" << std::endl;
		fileOutStream << "\t\t<PlayerRidingYoshi>" << ridingYoshi << "</PlayerRidingYoshi>" << std::endl;
		fileOutStream << "\t\t<PlayerPowerupState>" << playerPowerupState << "</PlayerPowerupState>" << std::endl;
		fileOutStream << "\t\t<TimeRemaining>" << timeRemaining << "</TimeRemaining>" << std::endl;
		fileOutStream << "\t\t<CoinsCollected>" << numberOfCoinsCollected << "</CoinsCollected>" << std::endl;
		fileOutStream << "\t\t<AllDragonCoinsCollected>" << allDragonCoinsCollected << "</AllDragonCoinsCollected>" << std::endl;

		if (writeStartInfo)
		{
			fileOutStream << "\t</Start>" << std::endl;
		}
		else
		{
			fileOutStream << "\t</End>" << std::endl;
			fileOutStream << "</Session>" << std::endl;
		}
	}

	fileOutStream.close();
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

	m_AllSessionInfo = stringStream.str();
	m_TotalSessionsWithInfo = GetNumberOfSessions();
	m_CurrentSessionInfo = GetReadableSessionInfo(m_CurrentSessionIndex);
}

int GameSession::GetNumberOfSessions()
{
	int numberOfSessions = 0;
	int currentIndex = -1;
	do
	{
		currentIndex = m_AllSessionInfo.find("<Session>", currentIndex + 1);
		numberOfSessions++;
	} while (currentIndex != std::string::npos);

	numberOfSessions--;

	return numberOfSessions;
}

std::string GameSession::GetReadableSessionInfo(int sessionIndex)
{
	int sessionTagStart;
	int sessionsFound = 0;
	int currentIndex = -1;
	do
	{
		currentIndex = m_AllSessionInfo.find("<Session>", currentIndex + 1);
		sessionsFound++;
	} while (currentIndex != std::string::npos && m_TotalSessionsWithInfo - sessionsFound > sessionIndex);

	sessionTagStart = currentIndex;
	int sessionTagEnd = m_AllSessionInfo.find("</Session>", sessionTagStart);

	if (sessionTagStart != std::string::npos)
	{
		std::string currentSessionRaw;
		if (sessionTagEnd != std::string::npos)
		{
			currentSessionRaw = m_AllSessionInfo.substr(sessionTagStart, sessionTagEnd - sessionTagStart);
		}
		else
		{
			currentSessionRaw = m_AllSessionInfo.substr(sessionTagStart);
		}
		std::stringstream currentSessionStream;
		currentSessionStream << "Session " << (m_CurrentSessionIndex + 1) << "/" << m_TotalSessionsWithInfo << ":\n\n";

		int startTag = currentSessionRaw.find("<Start>") + std::string("<Start>").length();
		int startTagEnd = currentSessionRaw.find("</Start>") + std::string("</Start>").length();
		if (startTag != std::string::npos && startTagEnd != std::string::npos)
		{
			std::string startInfo = currentSessionRaw.substr(startTag, startTagEnd - startTag);
			currentSessionStream << "Started on:\n";
			currentSessionStream << GetSessionInfoContent(startInfo);
		}
		currentSessionStream << "\n";

		int endTag = currentSessionRaw.find("<End>") + std::string("<End>").length();
		int endTagEnd = currentSessionRaw.find("</End>") + std::string("</End>").length();
		if (endTag != std::string::npos && endTagEnd != std::string::npos)
		{
			std::string endInfo = currentSessionRaw.substr(endTag, endTagEnd - endTag);
			currentSessionStream << "Ended on:\n";
			currentSessionStream << GetSessionInfoContent(endInfo);
		}

		return currentSessionStream.str();
	}
	else
	{
		return "No session info yet";
	}
}

std::string GameSession::GetSessionInfoContent(std::string sessionString)
{
	std::stringstream sessionStream;

	sessionStream << GetTagContentIfAvailable(sessionString, "Date");
	sessionStream << GetTagContentIfAvailable(sessionString, "Time");
	sessionStream << GetTagContentIfAvailable(sessionString, "PlayerLives", "Player Lives: ");
	sessionStream << GetTagContentIfAvailable(sessionString, "Score", "Score: ");
	sessionStream << GetTagContentIfAvailable(sessionString, "CheckpointCleared", "Checkpoint Cleared: ");
	sessionStream << GetTagContentIfAvailable(sessionString, "PlayerRidingYoshi", "Riding Yoshi: ");
	sessionStream << GetTagContentIfAvailable(sessionString, "PlayerPowerupState", "Powerup State: ");
	sessionStream << GetTagContentIfAvailable(sessionString, "TimeRemaining", "Time Remaining: ");
	sessionStream << GetTagContentIfAvailable(sessionString, "CoinsCollected", "Coins Collected: ");
	sessionStream << GetTagContentIfAvailable(sessionString, "AllDragonCoinsCollected", "All Dragon Coins Collected: ");

	return sessionStream.str();
}

std::string GameSession::GetTagContentIfAvailable(std::string sessionString, std::string tagString, std::string formattedString)
{
	std::string playerLivesString = FileIO::GetTagContent(sessionString, tagString);
	if (playerLivesString.length() > 0)
	{
		return formattedString + playerLivesString + "\n";
	}
	return "";
}

void GameSession::NextSession()
{
	if (m_CurrentSessionIndex + 1 < m_TotalSessionsWithInfo)
	{
		m_CurrentSessionIndex++;
		m_CurrentSessionInfo = GetReadableSessionInfo(m_CurrentSessionIndex);
	}
}

void GameSession::PreviousSession()
{
	if (m_CurrentSessionIndex - 1 >= 0)
	{
		m_CurrentSessionIndex--;
		m_CurrentSessionInfo = GetReadableSessionInfo(m_CurrentSessionIndex);
	}
}

void GameSession::PaintCurrentSessionInfo()
{
	int x = Game::WIDTH - 107;
	int y = 40;

	GAME_ENGINE->SetFont(Game::Font9Ptr);
	GAME_ENGINE->SetColor(COLOR(10, 10, 10, 160));
	GAME_ENGINE->FillRect(x - 5, y - 5, x + 106, y + 140);

	int lineHeight = 11;
	GAME_ENGINE->SetColor(COLOR(255, 255, 255));
	std::stringstream stringStream(m_CurrentSessionInfo);
	std::string currentLine;
	while (getline(stringStream, currentLine))
	{
		GAME_ENGINE->DrawString(String(currentLine.c_str()), x, y);
		y += lineHeight;
	}
}

void GameSession::Reset()
{
	m_CurrentSessionIndex = 0;
	m_CurrentSessionInfo = GameSession::GetReadableSessionInfo(m_CurrentSessionIndex);
}

GameSession::GameSession()
{}

GameSession::~GameSession()
{}
