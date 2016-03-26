//-----------------------------------------------------------------
// Game Engine
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------

#pragma once


class PhysicsActor;
class GameSettings;
//-----------------------------------------------------------------
// AbstractGame Class
//-----------------------------------------------------------------
class AbstractGame
{
public : 	
	AbstractGame() 
	{
		// nothing to create
	}

	virtual ~AbstractGame() 
	{
		// nothing to destroy
	}

	// C++11 make the class non-copyable
	AbstractGame(const AbstractGame&) = delete;
	AbstractGame& operator=(const AbstractGame&) = delete;

	virtual void GameInitialize(GameSettings &gameSettingsRef) = 0; // pure virtual method declaration
	virtual void GameStart(void) = 0;
	virtual void GameEnd(void) = 0;
	virtual void GamePaint() = 0;
	virtual void GameTick(double deltaTime) = 0;
};