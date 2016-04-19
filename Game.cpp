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
#include "Player.h"
#include "Enumerations.h"

Font* Game::Font12Ptr = nullptr;
Font* Game::Font9Ptr = nullptr;

MATRIX3X2 Game::matIdentity;

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
	SoundManager::SetMuted(true);

	// TODO: Add mario fonts
	Game::Font12Ptr = new Font(String("consolas"), 12);
	Game::Font9Ptr = new Font(String("consolas"), 9);

	matIdentity = MATRIX3X2::CreateScalingMatrix(WINDOW_SCALE);

#ifdef DEBUG_ZOOM_OUT
	matIdentity = MATRIX3X2::CreateScalingMatrix(0.65) * MATRIX3X2::CreateTranslationMatrix(150, 0);
#endif

	m_LevelPtr = new Level();

	m_AllSessionInfo = ReadSessionInfoFromFile();
	m_TotalSessionsWithInfo = GetNumberOfSessions(m_AllSessionInfo);
	m_CurrentSessionInfo = GetReadableSessionInfo(m_CurrentSessionInfoShowingIndex);

	WriteSessionInfoToFile(true);

	Reset();
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
	if (SoundManager::IsInitialized() == false) return;
	if (sleeping)
	{
		m_WasMuted = SoundManager::IsMuted();
		SoundManager::SetMuted(sleeping);
	}
	else
	{
		SoundManager::SetMuted(m_WasMuted);
	}
}

void Game::GameEnd()
{
	WriteSessionInfoToFile(false);

	delete m_LevelPtr;

	delete Font12Ptr;
	delete Font9Ptr;

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
	
		GAME_ENGINE->SetFont(Font9Ptr);
		GAME_ENGINE->SetColor(COLOR(10, 10, 10, 160));
		GAME_ENGINE->FillRect(x - 5, y - 5, x + 106, y + 140);

		int dy = 11;
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

void Game::WriteSessionInfoToFile(bool writeStartInfo)
{
	std::ofstream fileOutStream;

	fileOutStream.open("Resources/GameSessions.txt", std::fstream::app);

	if (fileOutStream.fail() == false)
	{
		int playerLives = m_LevelPtr->GetPlayer()->GetLives();

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

				fileOutStream << std::setfill('0') ;
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

				fileOutStream << "\t\t<PlayerLives>";
				fileOutStream << std::setw(2) << playerLives;
				fileOutStream << "</PlayerLives>" << std::endl;

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
		currentSessionStream << "Session " << (m_CurrentSessionInfoShowingIndex + 1) << "/" << m_TotalSessionsWithInfo << ":\n\n";

		int startTag = currentSessionRaw.find("<Start>") + std::string("<Start>").length();
		if (startTag != std::string::npos)
		{
			currentSessionStream << "Started on:\n";
			currentSessionStream << FileIO::GetTagContent(currentSessionRaw, "Date", startTag) << "\n";
			currentSessionStream << FileIO::GetTagContent(currentSessionRaw, "Time", startTag) << "\n";

			std::string playerLivesString = FileIO::GetTagContent(currentSessionRaw, "PlayerLives", startTag);
			if (playerLivesString.length() > 0)
			{
				currentSessionStream << "Player lives: ";
				currentSessionStream << playerLivesString;
				currentSessionStream << "\n";
			}
		}
		currentSessionStream << "\n";

		int endTag = currentSessionRaw.find("<End>") + std::string("<End>").length();
		if (endTag != std::string::npos)
		{
			currentSessionStream << "Ended on:" << "\n";
			currentSessionStream << FileIO::GetTagContent(currentSessionRaw, "Date", endTag) << "\n";
			currentSessionStream << FileIO::GetTagContent(currentSessionRaw, "Time", endTag) << "\n";

			std::string playerLivesString = FileIO::GetTagContent(currentSessionRaw, "PlayerLives", endTag);
			if (playerLivesString.length() > 0)
			{
				currentSessionStream << "Player lives: ";
				currentSessionStream << playerLivesString;
				currentSessionStream << "\n";
			}
		}

		return currentSessionStream.str();
	}
	else
	{
		return "No session info yet";
	}
}
