//-----------------------------------------------------------------
// Game Engine Object
// C++ Header - $projectname$.h - version v2_18a March 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

//-----------------------------------------------------------------
// Student data
// Name:
// Group:
//-----------------------------------------------------------------

#pragma once

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------

#include "Resource.h"	
#include "AbstractGame.h"

//-----------------------------------------------------------------
// $projectname$ Class																
//-----------------------------------------------------------------
class $projectname$ : public AbstractGame
{
public:
	//---------------------------
	// Constructor(s)
	//---------------------------
	$projectname$();

	//---------------------------
	// Destructor
	//---------------------------
	virtual ~$projectname$();

	// C++11 make the class non-copyable
	$projectname$(const $projectname$&) = delete;
	$projectname$& operator=(const $projectname$&) = delete;

	//---------------------------
	// General Methods
	//---------------------------

	virtual void GameInitialize(GameSettings &gameSettingsRef);
	virtual void GameStart();
	virtual void GameEnd();
	virtual void GameTick(double deltaTime);
	virtual void GamePaint();

	// -------------------------
	// Public Member functions
	// -------------------------

private:
	// -------------------------
	// Private Member functions
	// -------------------------

	// -------------------------
	// Private Datamembers
	// -------------------------

};
