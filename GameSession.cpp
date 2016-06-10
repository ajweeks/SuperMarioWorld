#include "stdafx.h"

#include "GameSession.h"
#include "Enumerations.h"
#include "Level.h"
#include "Player.h"
#include "Game.h"
#include "FileIO.h"
#include "Keybindings.h"

size_t GameSession::m_CurrentSessionShowingIndex = 0;
size_t GameSession::m_NumberOfSessions = 0;
std::string GameSession::m_AllSessionsInfoString = "";
SessionInfoPair GameSession::m_CurrentSessionInfoRecording = {};
std::vector<SessionInfoPair> GameSession::m_AllSessionInfoArr;

COLOR GameSession::OFF_WHITE = COLOR(245, 245, 250);
COLOR GameSession::GREEN = COLOR(16, 181, 16);
COLOR GameSession::RED = COLOR(193, 36, 36);

std::vector<int> GameSession::m_DaysOfWeekArr;
bool GameSession::m_DaysOfWeekArrGenerated = false;
bool GameSession::m_ShouldGenerateDaysOfWeek = false;

GameSession::GameSession()
{}

GameSession::~GameSession()
{}

void GameSession::RecordStartSessionInfo(Level* levelPtr)
{
	RecordSessionInfo(m_CurrentSessionInfoRecording.sessionInfoStart, levelPtr);

	m_NumberOfSessions = GetNumberOfSessions();
	m_AllSessionInfoArr.push_back(FindSessionInfoPair(0));

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
	sessionInfo.m_DragonCoinsCollected = levelPtr->GetPlayer()->GetDragonCoinsCollected();
}

void GameSession::WriteSessionInfoToFile(Level* levelPtr)
{
	RecordSessionInfo(m_CurrentSessionInfoRecording.sessionInfoEnd, levelPtr);

	std::ofstream fileOutStream;

	fileOutStream.open("Resources/GameSessions.txt", std::fstream::app);

	if (fileOutStream.fail() == false)
	{
		fileOutStream << "<Session>" << std::endl;

		fileOutStream << "\t<Start>" << std::endl;
		fileOutStream << GetSessionInfoMarkedUp(m_CurrentSessionInfoRecording.sessionInfoStart);
		fileOutStream << "\t</Start>" << std::endl;

		fileOutStream << "\t<End>" << std::endl;
		fileOutStream << GetSessionInfoMarkedUp(m_CurrentSessionInfoRecording.sessionInfoEnd);
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
	stringStream << "\t\t<DragonCoinsCollected>" << sessionInfo.m_DragonCoinsCollected << "</DragonCoinsCollected>" << std::endl;

	return stringStream.str();
}

void GameSession::ReadSessionInfoFromFile()
{
	std::ifstream fileInStream;
	std::stringstream stringStream;

	const std::string filePath = "Resources/GameSessions.txt";

	fileInStream.open(filePath);

	if (!fileInStream) // The file hasn't yet been created
	{
		std::ofstream fileOutStream;
		fileOutStream.open(filePath); // Create the file
		fileOutStream.close();

		fileInStream.open(filePath);
	}

	if (fileInStream.fail() == false)
	{
		std::string line;
		while (fileInStream.eof() == false)
		{
			std::getline(fileInStream, line);
			stringStream << line << "\n";
		}
	}

	fileInStream.close();

	m_AllSessionsInfoString = stringStream.str();
	// Remove whitespace
	m_AllSessionsInfoString.erase(std::remove_if(m_AllSessionsInfoString.begin(), m_AllSessionsInfoString.end(), isspace), m_AllSessionsInfoString.end());
}

int GameSession::GetNumberOfSessions()
{
	int sessionsFound = 0;
	int currentIndex = -1;
	while ((currentIndex = m_AllSessionsInfoString.find("<Session>", currentIndex + 1)) != std::string::npos)
	{
		++sessionsFound;
	}
	return sessionsFound;
}

void GameSession::ReadAllSessionData()
{
	m_AllSessionInfoArr.resize(m_NumberOfSessions);

	int currentIndexInFileString = 0;
	for (size_t i = 0; i < m_NumberOfSessions-1; ++i)
	{
		const int currentArrIndex = m_NumberOfSessions - i - 1;
		const int sessionTagStart = currentIndexInFileString;
		const int sessionTagEnd = m_AllSessionsInfoString.find("</Session>", sessionTagStart);

		if (m_AllSessionInfoArr[currentArrIndex].sessionInfoStart.m_Date.empty() == false) // This session has already been filled in
		{
			if (sessionTagEnd != -1)
			{
				currentIndexInFileString = sessionTagEnd + std::string("</Session>").length();
			}
		}
		else
		{
			if (sessionTagStart != std::string::npos && sessionTagEnd != std::string::npos)
			{
				const std::string currentSessionString = m_AllSessionsInfoString.substr(sessionTagStart, sessionTagEnd - sessionTagStart);
			
				m_AllSessionInfoArr[currentArrIndex] = GetSessionInfoPair(currentSessionString, currentArrIndex);

				currentIndexInFileString = sessionTagEnd + std::string("</Session>").length();
			}
		}
	}
}

SessionInfoPair GameSession::FindSessionInfoPair(size_t sessionIndex)
{
	if (sessionIndex < m_AllSessionInfoArr.size()) return m_AllSessionInfoArr[sessionIndex];

	int sessionsFound = 0;
	int currentIndex = -1;
	do
	{
		currentIndex = m_AllSessionsInfoString.find("<Session>", currentIndex + 1);
		sessionsFound++;
	} while (currentIndex != std::string::npos && m_NumberOfSessions - sessionsFound > sessionIndex);

	int sessionTagStart = currentIndex;
	int sessionTagEnd = m_AllSessionsInfoString.find("</Session>", sessionTagStart);

	if (sessionTagStart != std::string::npos && sessionTagEnd != std::string::npos)
	{
		const std::string currentSessionString = m_AllSessionsInfoString.substr(sessionTagStart, sessionTagEnd - sessionTagStart);
		
		return GetSessionInfoPair(currentSessionString, sessionIndex);
	}
	return {};
}

SessionInfoPair GameSession::GetSessionInfoPair(const std::string& sessionString, int sessionIndex)
{
	SessionInfoPair sessionInfoPair;

	const int startTag = sessionString.find("<Start>") + std::string("<Start>").length();
	const int startTagEnd = sessionString.find("</Start>");
	if (startTag != std::string::npos && startTagEnd != std::string::npos)
	{
		const std::string startInfoString = sessionString.substr(startTag, startTagEnd - startTag);
		sessionInfoPair.sessionInfoStart = GetSessionInfo(startInfoString);
	}

	const int endTag = sessionString.find("<End>") + std::string("<End>").length();
	const int endTagEnd = sessionString.find("</End>");
	if (endTag != std::string::npos && endTagEnd != std::string::npos)
	{
		const std::string endInfoString = sessionString.substr(endTag, endTagEnd - endTag);
		sessionInfoPair.sessionInfoEnd = GetSessionInfo(endInfoString);
	}
	sessionInfoPair.m_SessionIndex = sessionIndex;

	return sessionInfoPair;
}

SessionInfo GameSession::GetSessionInfo(const std::string& sessionString)
{
	SessionInfo sessionInfo = {};
	
	sessionInfo.m_Date = FileIO::GetTagContent(sessionString, "Date");
	sessionInfo.m_Time = FileIO::GetTagContent(sessionString, "Time");

	const std::string playerPowerupStateStr = FileIO::GetTagContent(sessionString, "PlayerPowerupState");
	if (playerPowerupStateStr.length() > 0) sessionInfo.m_PlayerPowerupState = playerPowerupStateStr;

	const std::string playerLivesStr = FileIO::GetTagContent(sessionString, "PlayerLives");
	if (playerLivesStr.length() > 0) sessionInfo.m_PlayerLives = stoi(playerLivesStr);

	const std::string playerScoreStr = FileIO::GetTagContent(sessionString, "Score");
	if (playerScoreStr.length() > 0) sessionInfo.m_PlayerScore = stoi(playerScoreStr);

	const std::string timeRemainingStr = FileIO::GetTagContent(sessionString, "TimeRemaining");
	if (timeRemainingStr.length() > 0) sessionInfo.m_TimeRemaining = stoi(timeRemainingStr);

	const std::string coinsCollectedStr = FileIO::GetTagContent(sessionString, "CoinsCollected");
	if(coinsCollectedStr.length() > 0) sessionInfo.m_CoinsCollected = stoi(coinsCollectedStr);

	const std::string checkpointClearedStr = FileIO::GetTagContent(sessionString, "CheckpointCleared");
	if (checkpointClearedStr.length() > 0) sessionInfo.m_CheckpointCleared = FileIO::StringToBool(checkpointClearedStr);

	const std::string playerRidingYoshiStr = FileIO::GetTagContent(sessionString, "PlayerRidingYoshi");
	if (playerRidingYoshiStr.length() > 0) sessionInfo.m_PlayerRidingYoshi = FileIO::StringToBool(playerRidingYoshiStr);

	const std::string allDragonCoinsCollectedStr = FileIO::GetTagContent(sessionString, "DragonCoinsCollected");
	if (allDragonCoinsCollectedStr.length() > 0) sessionInfo.m_DragonCoinsCollected = FileIO::StringToBool(allDragonCoinsCollectedStr);

	//sessionInfo.m_Initialized = true;

	return sessionInfo;
}

void GameSession::Tick(double deltaTime)
{
	if (m_ShouldGenerateDaysOfWeek)
	{
		CountDaysOfWeek();
	}

	if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::GENERATE_BAR_GRAPH) &&
		m_DaysOfWeekArrGenerated == false && 
		m_NumberOfSessions > 0)
	{
		CountDaysOfWeek();
		return;
	}

	if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::SHOW_NEXT_INFO_SESSION) ||
		(GAME_ENGINE->IsKeyboardKeyDown(Keybindings::SHOW_NEXT_INFO_SESSION) &&
			GAME_ENGINE->IsKeyboardKeyDown(Keybindings::SCROLL_THROUGH_SESSIONS)))
	{
		ShowNextSession();
	}
	if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::SHOW_PREVIOUS_INFO_SESSION) ||
		(GAME_ENGINE->IsKeyboardKeyDown(Keybindings::SHOW_PREVIOUS_INFO_SESSION) &&
			GAME_ENGINE->IsKeyboardKeyDown(Keybindings::SCROLL_THROUGH_SESSIONS)))
	{
		ShowPreviousSession();
	}
}

void GameSession::Paint()
{
	MATRIX3X2 matPrevView = GAME_ENGINE->GetViewMatrix();
	GAME_ENGINE->SetViewMatrix(Game::matIdentity);

	// BACKGROUND ROWS
	int rows = 12;
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
	GAME_ENGINE->DrawString(String("Current session: ") + String(m_NumberOfSessions - m_CurrentSessionShowingIndex) +
		String("/") + String(m_NumberOfSessions), x, y);
	y += 16;

	// HEADINGS
	GAME_ENGINE->SetFont(Game::Font12Ptr);
	GAME_ENGINE->DrawString(String("Start"), x + 15, y);
	GAME_ENGINE->DrawString(String("End"), x + 170, y);

	y += 20;
	GAME_ENGINE->DrawLine(10, y, Game::WIDTH - 20, y);

	// MAIN INFO
	GAME_ENGINE->SetFont(Game::Font9Ptr);
	
	const SessionInfoPair m_CurrentSessionInfoShowing = m_AllSessionInfoArr[m_CurrentSessionShowingIndex];
	if (m_CurrentSessionInfoShowing.sessionInfoStart.m_Date == "")
	{
		GAME_ENGINE->SetColor(OFF_WHITE);
		GAME_ENGINE->DrawString(String("No game sessions have been recorded yet!"), x, y);
		return;
	}

	PaintInfoString("Date: ",
		m_CurrentSessionInfoShowing.sessionInfoStart.m_Date,
		m_CurrentSessionInfoShowing.sessionInfoEnd.m_Date,
		x, y);

	PaintInfoString("Time: ",
		m_CurrentSessionInfoShowing.sessionInfoStart.m_Time,
		m_CurrentSessionInfoShowing.sessionInfoEnd.m_Time,
		x, y);
	std::string timeDurationStr = GetTimeDuration(m_CurrentSessionInfoShowing.sessionInfoStart.m_Time, m_CurrentSessionInfoShowing.sessionInfoEnd.m_Time);
	GAME_ENGINE->DrawString(String("(") + String(timeDurationStr.c_str())+ String(")"), x + COL_WIDTH, y);
	y += LINE_HEIGHT;

	PaintInfoString("Player lives: ",
		m_CurrentSessionInfoShowing.sessionInfoStart.m_PlayerLives,
		m_CurrentSessionInfoShowing.sessionInfoEnd.m_PlayerLives,
		x, y);

	PaintInfoString("Score: ",
		m_CurrentSessionInfoShowing.sessionInfoStart.m_PlayerScore,
		m_CurrentSessionInfoShowing.sessionInfoEnd.m_PlayerScore,
		x, y);

	PaintInfoString("Coins: ",
		m_CurrentSessionInfoShowing.sessionInfoStart.m_CoinsCollected,
		m_CurrentSessionInfoShowing.sessionInfoEnd.m_CoinsCollected,
		x, y);

	PaintInfoString("Dragon coins: ",
		m_CurrentSessionInfoShowing.sessionInfoStart.m_DragonCoinsCollected,
		m_CurrentSessionInfoShowing.sessionInfoEnd.m_DragonCoinsCollected,
		x, y);

	PaintInfoString("Player powerup state: ",
		m_CurrentSessionInfoShowing.sessionInfoStart.m_PlayerPowerupState,
		m_CurrentSessionInfoShowing.sessionInfoEnd.m_PlayerPowerupState,
		x, y);

	PaintInfoString("Time remaining: ",
		m_CurrentSessionInfoShowing.sessionInfoStart.m_TimeRemaining,
		m_CurrentSessionInfoShowing.sessionInfoEnd.m_TimeRemaining,
		x, y);

	PaintInfoString("Checkpoint cleared: ",
		m_CurrentSessionInfoShowing.sessionInfoStart.m_CheckpointCleared,
		m_CurrentSessionInfoShowing.sessionInfoEnd.m_CheckpointCleared,
		x, y);

	PaintInfoString("Riding yoshi: ",
		m_CurrentSessionInfoShowing.sessionInfoStart.m_PlayerRidingYoshi,
		m_CurrentSessionInfoShowing.sessionInfoEnd.m_PlayerRidingYoshi,
		x, y);

	x = 12;
	y = 170;
	if (m_DaysOfWeekArrGenerated)
	{
		GAME_ENGINE->SetColor(OFF_WHITE);
		GAME_ENGINE->DrawString(String("Number of sessions:"), x, y);
		y += 12;
		PaintDaysOfWeekBarGraph(m_DaysOfWeekArr, x, y);

		x += 200;
		y += 12;
		PaintDaysOfWeekPieChart(m_DaysOfWeekArr, x, y);
	}
	else if (m_ShouldGenerateDaysOfWeek)
	{
		GAME_ENGINE->SetColor(OFF_WHITE);
		GAME_ENGINE->DrawString(String("Generating cool graph ..."), x, y);
	}
	else
	{
		GAME_ENGINE->SetColor(OFF_WHITE);
		GAME_ENGINE->DrawString(String("Press F2 to generate cool graph!"), x, y);
	}

	GAME_ENGINE->SetViewMatrix(matPrevView);
}

void GameSession::PaintInfoString(const std::string& preString, const std::string& value1, const std::string& value2, int& x, int& y, bool positive)
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

void GameSession::PaintInfoString(const std::string& preString, int value1, int value2, int& x, int& y)
{
	if (value1 == -1 || value2 == -1) return;

	PaintInfoString(preString, std::to_string(value1), std::to_string(value2), x, y, value1 < value2);
}

void GameSession::PaintInfoString(const std::string& preString, const std::string& value1, const std::string& value2, int& x, int& y)
{
	if (!value1.compare("") || !value2.compare("")) return;

	PaintInfoString(preString, value1, value2, x, y, value1.compare(value2) < 0);
}

void GameSession::PaintInfoString(const std::string& preString, bool value1, bool value2, int& x, int& y)
{
	if (int(value1) == -1 || int(value2) == -1) return;
	
	PaintInfoString(preString, FileIO::BoolToString(value1), FileIO::BoolToString(value2), x, y, value2 && !value1);
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

void GameSession::PaintDaysOfWeekBarGraph(const std::vector<int>& daysOfWeekArr, int x, int y)
{
	static const int DAYS_IN_A_WEEK = 7;
	static const std::string daysNames[DAYS_IN_A_WEEK] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	int mostDays = 0;
	for (size_t i = 0; i < daysOfWeekArr.size(); ++i)
		if (daysOfWeekArr[i] > mostDays) mostDays = daysOfWeekArr[i];

	const int barWidth = 16;
	const int maxBarHeight = (Game::HEIGHT - y) - 14;
	const int spacing = 4;

	// Background rectangle and horizontal lines
	GAME_ENGINE->SetColor(COLOR(150, 150, 150, 80));
	GAME_ENGINE->FillRect(x - 2, y - 2, x + (barWidth + spacing) * DAYS_IN_A_WEEK - spacing + 2, y + maxBarHeight + 2);

	GAME_ENGINE->SetColor(COLOR(200, 200, 200, 50));
	const int numOfHorizontalLines = 5;
	for (int i = 0; i < numOfHorizontalLines; ++i)
	{
		int lineY = y + (maxBarHeight / (numOfHorizontalLines)) * (i + 1) - 2;
		GAME_ENGINE->DrawLine(x - 2, lineY, x + (barWidth + spacing) * DAYS_IN_A_WEEK - spacing + 2, lineY, 0.5);
	}

	// Bars + values
	for (size_t i = 0; i < daysOfWeekArr.size(); ++i)
	{
		int barHeight = int((double(daysOfWeekArr[i]) / double(mostDays)) * maxBarHeight);
		RECT2 barRect(x, y + maxBarHeight - barHeight, x + barWidth, y + maxBarHeight);
		double barPercent = (double(barHeight) / double(maxBarHeight));
		int green = int(55 + barPercent * 200);
		int red = int(255 - barPercent * 200);
		GAME_ENGINE->SetColor(COLOR(red, green, 80));
		GAME_ENGINE->FillRect(barRect);

		GAME_ENGINE->SetColor(OFF_WHITE);
		if (daysOfWeekArr[i] > 0)
		{
			GAME_ENGINE->SetFont(Game::Font6Ptr);
			GAME_ENGINE->DrawString(String(daysOfWeekArr[i]), x + 3, y + maxBarHeight - 7);
		}

		GAME_ENGINE->SetFont(Game::Font9Ptr);
		GAME_ENGINE->DrawString(String(daysNames[i].c_str()), x, y + maxBarHeight + 2);
		x += barWidth + spacing;
	}
}

void GameSession::PaintDaysOfWeekPieChart(const std::vector<int>& daysOfWeekArr, int centerX, int centerY)
{
	static const std::string daysNames[7] = { "Su", "M", "Tu", "W", "Th", "F", "Sa" };

	const int radius = 26;

	GAME_ENGINE->SetFont(Game::Font6Ptr);
	double currentAngle = 0.0;
	for (size_t i = 0; i < daysOfWeekArr.size(); ++i)
	{
		if (daysOfWeekArr[i] > 0)
		{
			double pieSliceAngle = (double(daysOfWeekArr[i]) / double(m_NumberOfSessions)) * M_PI  * 2;
			int green = int(50 + (i / 7.0) * 50);
			int red = int(55 + (i / 7.0) * 200);
			int blue = int((7.0 - i / 7.0) * 200);
			GAME_ENGINE->SetColor(COLOR(red, green, blue));
			const int linesPerRadian = radius * 3;
			int lines = int(linesPerRadian * pieSliceAngle);
			for (int i = 0; i < lines; ++i)
			{
				GAME_ENGINE->DrawLine(centerX, centerY, 
					centerX + cos(currentAngle) * radius,
					centerY + sin(currentAngle) * radius, 0.4);
				currentAngle += (pieSliceAngle / lines);

			}

			GAME_ENGINE->SetColor(OFF_WHITE);
			GAME_ENGINE->DrawString(String(daysNames[i].c_str()), 
				centerX + cos(currentAngle - pieSliceAngle / 2 - 0.1) * (radius - 7) - 4,
				centerY + sin(currentAngle - pieSliceAngle / 2 - 0.1) * (radius - 7) - 5);
		}
	}

	GAME_ENGINE->SetColor(COLOR(10, 10, 10));
	GAME_ENGINE->DrawEllipse(centerX, centerY, radius, radius, 0.8);
}

void GameSession::CountDaysOfWeek()
{
	// When first called, we want to paint one more frame, to let the user know
	// their input went through before the long wait
	if (m_ShouldGenerateDaysOfWeek == false) 
	{
		m_ShouldGenerateDaysOfWeek = true;
		return;
	}
	else
	{
		m_ShouldGenerateDaysOfWeek = false;
	}
	ReadAllSessionData();

	std::vector<int> daysOfTheWeekArr(7); // [Sunday, Monday, ..., Saturday]

	static const int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 }; // Magic array
	for (size_t i = 0; i < m_NumberOfSessions; ++i)
	{
		const SessionInfoPair currentSessionInfoPair = m_AllSessionInfoArr[i];
		const std::string dateString = currentSessionInfoPair.sessionInfoStart.m_Date;

		int year = stoi(dateString.substr(0, 4));
		int month = stoi(dateString.substr(5, 7));
		int day = stoi(dateString.substr(8, 10));

		year -= month < 3;
		int dayOfWeek = (year + year / 4 - year / 100 + year / 400 + t[month - 1] + day) % 7; // Magic formula
		++daysOfTheWeekArr[dayOfWeek];
	}

	m_DaysOfWeekArrGenerated = true;
	m_DaysOfWeekArr = daysOfTheWeekArr;
}

void GameSession::ShowNextSession()
{
	if (m_CurrentSessionShowingIndex + 1 < m_NumberOfSessions)
	{
		m_CurrentSessionShowingIndex++;
		
		if (m_AllSessionInfoArr.size() <= m_CurrentSessionShowingIndex)
		{
			m_AllSessionInfoArr.push_back(FindSessionInfoPair(m_CurrentSessionShowingIndex));
		}
	}
}

void GameSession::ShowPreviousSession()
{
	if (m_CurrentSessionShowingIndex - 1 >= 0)
	{
		m_CurrentSessionShowingIndex--;
	}
}

void GameSession::Reset()
{
	m_CurrentSessionShowingIndex = 0;
}
