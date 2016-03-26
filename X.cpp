//-----------------------------------------------------------------
// Game Engine Object
// C++ Source - $projectname$.cpp - version v2_18a March 2016  
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

// this include must be the first include line of every cpp file (due to using precompiled header)
#include "stdafx.h"		

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "$projectname$.h"																				

//-----------------------------------------------------------------
// Defines
//-----------------------------------------------------------------
#define GAME_ENGINE (GameEngine::GetSingleton())

//-----------------------------------------------------------------
// $projectname$ methods																				
//-----------------------------------------------------------------

$projectname$::$projectname$()
{
	// nothing to create
}

$projectname$::~$projectname$()
{
	// nothing to destroy
}

void $projectname$::GameInitialize(GameSettings &gameSettingsRef)
{
	gameSettingsRef.SetWindowTitle(String("$projectname$ - Name, First name - group"));
	gameSettingsRef.SetWindowWidth(842);
	gameSettingsRef.SetWindowHeight(480);
	gameSettingsRef.EnableConsole(false);
	gameSettingsRef.EnableAntiAliasing(false);
}

void $projectname$::GameStart()
{
	// Insert the code that needs to be executed at the start of the game
}

void $projectname$::GameEnd()
{
	// Insert the code that needs to be executed at the closing of the game
}

void $projectname$::GameTick(double deltaTime)
{
	// Insert the code that needs to be executed, EXCEPT for paint commands (see next method)
}

void $projectname$::GamePaint()
{
	// Insert the code that needs to be executed each time a new frame needs to be drawn to the screen
}
