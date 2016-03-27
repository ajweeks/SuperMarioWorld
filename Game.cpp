//-----------------------------------------------------------------
// Game Engine Object
// C++ Source - Game.cpp - version v2_18 feb 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

// this include must be the first include line of every cpp file (due to using precompiled header)
#include "stdafx.h"		

#include "Game.h"		
#include "SpriteSheetManager.h"
#include "LevelData.h"

#define GAME_ENGINE (GameEngine::GetSingleton())

Font* Game::Font16Ptr = nullptr;
Font* Game::Font24Ptr = nullptr;

Game::Game()
{
}

Game::~Game()
{
}

void Game::GameInitialize(GameSettings &gameSettingsRef)
{
	gameSettingsRef.SetWindowTitle(String("Super Mario World - Weeks, Andrew - 1DAE06"));
	gameSettingsRef.SetWindowWidth(512);
	gameSettingsRef.SetWindowHeight(448);
	gameSettingsRef.EnableConsole(false);
	gameSettingsRef.EnableAntiAliasing(false);
}

void Game::GameStart()
{
	SpriteSheetManager::Load();
	Game::Font16Ptr = new Font(String("smwtextfontpro"), 16);
	Game::Font24Ptr = new Font(String("smwtextfontpro"), 24);

	m_LevelPtr = new Level();

	m_AllSessionInfo = ReadSessionInfoFromFile();
	m_TotalSessionsWithInfo = GetNumberOfSessions(m_AllSessionInfo);
	m_CurrentSessionInfo = GetReadableSessionInfo(m_CurrentSessionInfoShowingIndex);

	GetSystemTime(&m_StartTime);
}

void Game::GameEnd()
{
	delete m_LevelPtr;

	delete Font16Ptr;
	delete Font24Ptr;

	WriteSessionInfoToFile();

	LevelData::Unload();
	SpriteSheetManager::Unload();
}

void Game::GameTick(double deltaTime)
{
	if (GAME_ENGINE->IsKeyboardKeyPressed(VK_ESCAPE))
	{
		m_Paused = !m_Paused;
		// TODO: Find a better way to pause the world
		m_LevelPtr->TogglePaused(m_Paused);
	}
	if (m_Paused) return;

	m_LevelPtr->Tick(deltaTime);

	if (GAME_ENGINE->IsKeyboardKeyPressed('P'))
	{
		m_RenderDebugOverlay = !m_RenderDebugOverlay;
		GAME_ENGINE->EnablePhysicsDebugRendering(m_RenderDebugOverlay);
	}

	if (GAME_ENGINE->IsKeyboardKeyPressed('R'))
	{
		m_LevelPtr->Reset();
	}

	if (GAME_ENGINE->IsKeyboardKeyPressed('I'))
	{
		m_ShowingSessionInfo = !m_ShowingSessionInfo;
	}
	if (m_ShowingSessionInfo)
	{
		if (GAME_ENGINE->IsKeyboardKeyPressed(VK_NEXT)) // Page Down
		{
			if (m_CurrentSessionInfoShowingIndex + 1 < m_TotalSessionsWithInfo)
			{
				m_CurrentSessionInfoShowingIndex++;
				m_CurrentSessionInfo = GetReadableSessionInfo(m_CurrentSessionInfoShowingIndex);
			}
		}
		if (GAME_ENGINE->IsKeyboardKeyPressed(VK_PRIOR)) // Page Up
		{
			if (m_CurrentSessionInfoShowingIndex - 1 >= 0)
			{
				m_CurrentSessionInfoShowingIndex--;
				m_CurrentSessionInfo = GetReadableSessionInfo(m_CurrentSessionInfoShowingIndex);
			}
		}
	}
}

void Game::GamePaint()
{
	m_LevelPtr->Paint();

	MATRIX3X2 matPrevious = GAME_ENGINE->GetViewMatrix();
	GAME_ENGINE->SetViewMatrix(MATRIX3X2::CreateIdentityMatrix());
	if (m_Paused)
	{
		GAME_ENGINE->SetColor(COLOR(255, 255, 255));
		GAME_ENGINE->DrawRect(0, 0, GAME_ENGINE->GetWidth(), GAME_ENGINE->GetHeight(), 6);
	}
	if (m_ShowingSessionInfo)
	{
		int x = GAME_ENGINE->GetWidth() - 110;
		int y = 10;
		int dy = 15;
		GAME_ENGINE->SetColor(COLOR(255,255,255));
		std::stringstream stringStream(m_CurrentSessionInfo);
		std::string currentLine;
		while (getline(stringStream, currentLine))
		{
			GAME_ENGINE->DrawString(String(currentLine.c_str()), x, y);
			y += dy;
		}
	}

	GAME_ENGINE->SetViewMatrix(matPrevious);
}

void Game::WriteSessionInfoToFile()
{
	if (m_StartTime.wYear == 0)
	{
		OutputDebugString(String("ERROR: m_StartTime was not intialized! Can not write useful info to GameSessions.txt"));
		return;
	}

	std::ofstream fileOutStream;

	fileOutStream.open("Resources/GameSessions.txt", std::fstream::app);

	if (fileOutStream.fail() == false)
	{
		fileOutStream << "<Session>" << std::endl;
		fileOutStream << "\t<Start>" << std::endl;
		
		fileOutStream << std::setfill('0') ;
		fileOutStream << "\t\t<Date>";
		fileOutStream << m_StartTime.wYear << ":";
		fileOutStream << std::setw(2) << m_StartTime.wMonth << ":";
		fileOutStream << std::setw(2) << m_StartTime.wDay;
		fileOutStream << "</Date>" << std::endl;

		fileOutStream << "\t\t<Time>"; 
		fileOutStream << std::setw(2) << m_StartTime.wHour << ":";
		fileOutStream << std::setw(2) << m_StartTime.wMinute << ":" ;
		fileOutStream << std::setw(2) << m_StartTime.wSecond;
		fileOutStream << "</Time>" << std::endl;

		fileOutStream << "\t</Start>" << std::endl;
		fileOutStream << "\t<End>" << std::endl;
	
		SYSTEMTIME time;
		GetSystemTime(&time);
		fileOutStream << "\t\t<Date>";
		fileOutStream << time.wYear << ":";
		fileOutStream << std::setw(2) << time.wMonth << ":";
		fileOutStream << std::setw(2) << time.wDay;
		fileOutStream << "</Date>" << std::endl;

		fileOutStream << "\t\t<Time>";
		fileOutStream << std::setw(2) << time.wHour << ":";
		fileOutStream << std::setw(2) << time.wMinute << ":";
		fileOutStream << std::setw(2) << time.wSecond;
		fileOutStream << "</Time>" << std::endl;

		fileOutStream << "\t</End>" << std::endl;
		fileOutStream << "</Session>" << std::endl;
	}

	fileOutStream.close();
}

std::string Game::ReadSessionInfoFromFile()
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

	return stringStream.str();
}

int Game::GetNumberOfSessions(std::string allSessionInfo)
{
	int numberOfSessions = 0;
	int currentIndex = -1;
	do
	{
		currentIndex = allSessionInfo.find("<Session>", currentIndex + 1);
		numberOfSessions++;
	} while (currentIndex != std::string::npos);

	numberOfSessions--;

	return numberOfSessions;
}

std::string Game::GetReadableSessionInfo(int sessionIndex)
{
	int sessionTagStart;
	int sessionsFound = 0;
	int currentIndex = -1;
	do 
	{
		currentIndex = m_AllSessionInfo.find("<Session>", currentIndex+1);
		sessionsFound++;
	} 
	while (currentIndex != std::string::npos && m_TotalSessionsWithInfo - sessionsFound > sessionIndex);

	sessionTagStart = currentIndex;

	if (sessionTagStart != std::string::npos)
	{
		std::string currentSessionRaw = m_AllSessionInfo.substr(sessionTagStart);
		std::stringstream currentSessionStream;
		currentSessionStream << "Session " << (m_CurrentSessionInfoShowingIndex + 1) << "/" << m_TotalSessionsWithInfo << ":\n\n";

		int startTag = currentSessionRaw.find("<Start>") + std::string("<Start>").length();
		currentSessionStream << "Started on:" << "\n";

		int startDateBegin = currentSessionRaw.find("<Date>", startTag) + std::string("<Date>").length();
		int startDateEnd = currentSessionRaw.find("</Date>", startDateBegin);
		currentSessionStream << currentSessionRaw.substr(startDateBegin, startDateEnd - startDateBegin) << "\n";

		int startTimeBegin = currentSessionRaw.find("<Time>", startTag) + std::string("<Time>").length();
		int startTimeEnd = currentSessionRaw.find("</Time>", startTimeBegin);
		currentSessionStream << currentSessionRaw.substr(startTimeBegin, startTimeEnd - startTimeBegin) << "\n\n";


		int endTag = currentSessionRaw.find("<End>") + std::string("<Start>").length();
		currentSessionStream << "Ended on:" << "\n";

		int endDateStart = currentSessionRaw.find("<Date>", endTag) + std::string("<Date>").length();
		int endDateEnd = currentSessionRaw.find("</Date>", endDateStart);
		currentSessionStream << currentSessionRaw.substr(endDateStart, endDateEnd - endDateStart) << "\n";

		int endTimeStart = currentSessionRaw.find("<Time>", endTag) + std::string("<Time>").length();
		int endTimeEnd = currentSessionRaw.find("</Time>", endTimeStart);
		currentSessionStream << currentSessionRaw.substr(endTimeStart, endTimeEnd - endTimeStart) << "\n";

		return currentSessionStream.str();
	}
	else
	{
		return "No session info yet";
	}
}