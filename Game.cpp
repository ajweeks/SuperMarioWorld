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
#include "SoundManager.h"

Font* Game::Font12Ptr = nullptr;

// Not actually what this matrix will contain, unless Game::WINDOW_SCALE is set to 1
MATRIX3X2 Game::matIdentity = MATRIX3X2::CreateIdentityMatrix();

Game::Game()
{
}

Game::~Game()
{
}

void Game::GameInitialize(GameSettings &gameSettingsRef)
{
	gameSettingsRef.SetWindowTitle(String("Super Mario World - Weeks, Andrew - 1DAE06"));

	gameSettingsRef.SetWindowWidth(WIDTH * WINDOW_SCALE);
	gameSettingsRef.SetWindowHeight(HEIGHT * WINDOW_SCALE);

	gameSettingsRef.EnableConsole(false);
	gameSettingsRef.EnableAntiAliasing(false);

	// NOTE: false prevents screen tearing
	gameSettingsRef.EnableVSync(false);

	// TODO: find out how to customize the icon in the menu bar
	//WORD wIcon;

	// NOTE: This line makes the sprites render without an ugly 
	// black line around the edges, BUT the original game seems
	// to have used some amount of blurring, so this does make it 
	// look a little too crisp.
	GAME_ENGINE->SetBitmapInterpolationModeNearestNeighbor();
}

void Game::GameStart()
{
	SpriteSheetManager::Load();

	SoundManager::InitialzeSoundsAndSongs();
	SoundManager::SetMuted(false);

	// TODO: Add mario fonts
	Game::Font12Ptr = new Font(String("consolas"), 12);

	matIdentity = MATRIX3X2::CreateScalingMatrix(WINDOW_SCALE);

	m_LevelPtr = new Level();

	m_AllSessionInfo = ReadSessionInfoFromFile();
	m_TotalSessionsWithInfo = GetNumberOfSessions(m_AllSessionInfo);
	m_CurrentSessionInfo = GetReadableSessionInfo(m_CurrentSessionInfoShowingIndex);

	Reset();

	GetSystemTime(&m_StartTime);
}

void Game::Reset()
{
	m_LevelPtr->Reset();

	m_SecondsElapsed = 0.0;

	m_CurrentSessionInfoShowingIndex = 0;
	m_CurrentSessionInfo = GetReadableSessionInfo(m_CurrentSessionInfoShowingIndex);

	m_ShowingSessionInfo = false;

	m_RenderDebugOverlay = false;
	GAME_ENGINE->EnablePhysicsDebugRendering(m_RenderDebugOverlay);
}

void Game::GameSetSleeping(bool sleeping)
{
	SoundManager::SetMuted(sleeping);
}

void Game::GameWindowResize(int width, int height)
{
	// TODO: Fix window resizing
}

void Game::GameEnd()
{
	delete m_LevelPtr;

	delete Font12Ptr;

	WriteSessionInfoToFile();

	LevelData::Unload();
	SpriteSheetManager::Unload();

	SoundManager::UnloadSoundsAndSongs();
}

void Game::GameTick(double deltaTime)
{
	m_LevelPtr->Tick(deltaTime);

	if (GAME_ENGINE->IsKeyboardKeyPressed('P'))
	{
		// TODO: Find out why the game is so laggy when physics debug overlay is rendering
		m_RenderDebugOverlay = !m_RenderDebugOverlay;
		GAME_ENGINE->EnablePhysicsDebugRendering(m_RenderDebugOverlay);
	}

	if (GAME_ENGINE->IsKeyboardKeyPressed('M'))
	{
		SoundManager::ToggleMuted();
	}

	if (GAME_ENGINE->IsKeyboardKeyPressed('R'))
	{
		Reset();
	}

	if (GAME_ENGINE->IsKeyboardKeyPressed('I'))
	{
		m_ShowingSessionInfo = !m_ShowingSessionInfo;
	}
	if (m_ShowingSessionInfo)
	{
		if (GAME_ENGINE->IsKeyboardKeyPressed(VK_NEXT) || 
			(GAME_ENGINE->IsKeyboardKeyDown(VK_NEXT) && GAME_ENGINE->IsKeyboardKeyDown(VK_CONTROL))) // Page Down
		{
			if (m_CurrentSessionInfoShowingIndex + 1 < m_TotalSessionsWithInfo)
			{
				m_CurrentSessionInfoShowingIndex++;
				m_CurrentSessionInfo = GetReadableSessionInfo(m_CurrentSessionInfoShowingIndex);
			}
		}
		if (GAME_ENGINE->IsKeyboardKeyPressed(VK_PRIOR) || 
			(GAME_ENGINE->IsKeyboardKeyDown(VK_PRIOR) && GAME_ENGINE->IsKeyboardKeyDown(VK_CONTROL))) // Page Up
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
	GAME_ENGINE->SetViewMatrix(matIdentity);
	if (m_ShowingSessionInfo)
	{
		int x = Game::WIDTH - 107;
		int y = 40;
	
		GAME_ENGINE->SetFont(Font12Ptr);
		GAME_ENGINE->SetColor(COLOR(10, 10, 10, 160));
		GAME_ENGINE->FillRect(x - 5, y - 5, x + 106, y + 140);

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
