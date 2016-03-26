//-----------------------------------------------------------------
// Game Engine
// C++ Source - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------
#include "stdafx.h"
#include "../stdafx.h"


//------------------------------------------------------------------------------
// GameSettings class definitions. Encapsulated the user defined game settings
//------------------------------------------------------------------------------
GameSettings::GameSettings()
{
}

GameSettings::~GameSettings()
{
}

void GameSettings::SetWindowTitle(const String& titleRef)
{
	m_WindowTitle = String(titleRef);
}

void GameSettings::SetWindowWidth(int width)
{
	m_WindowWidth = width;
}

void GameSettings::SetWindowHeight(int height)
{
	m_WindowHeight = height;
}

void GameSettings::EnableConsole(bool state)
{
	m_bEnableConsole = state;
}

void GameSettings::EnableVSync(bool state)
{
	m_bVSync = state;
}

void GameSettings::EnableAntiAliasing(bool state)
{
	m_bEnableAntiAliasing = state;
}

//void GameSettings::EnablePhysicsDebugRendering(bool state)
//{
//	m_EnableDebugRendering = state;
//}

String GameSettings::GetWindowTitle()
{
	return m_WindowTitle;
}

int GameSettings::GetWindowWidth()
{
	return m_WindowWidth;
}

int GameSettings::GetWindowHeight()
{
	return m_WindowHeight;
}

bool GameSettings::IsConsoleEnabled()
{
	return m_bEnableConsole;
}

bool GameSettings::IsVSync()
{
	return m_bVSync;
}

bool GameSettings::IsAntiAliasingEnabled()
{
	return m_bEnableAntiAliasing;
}

//bool GameSettings::IsDebugRenderingEnabled()
//{
//	return m_EnableDebugRendering;
//}