//-----------------------------------------------------------------
// Game Engine
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

#pragma once

class GameSettings
{
public:
	virtual ~GameSettings(void);

	// C++11 make the class non-copyable
	GameSettings(const GameSettings&) = delete;
	GameSettings& operator=(const GameSettings&) = delete;

	//! Defines the title that will appear in the window
	void SetWindowTitle(const String& titleRef);
	//! Defines the width of the client area of the window (without borders)
	void SetWindowWidth(int width);
	//! Defines the height of the client area of the window (without borders)
	void SetWindowHeight(int height);
	//! Do you want a console?
	void EnableConsole(bool state);
	//! True locks the framerate to the display vertical frequency (60Hz in most cases)
	void EnableVSync(bool state);
	//! True enable smooth drawing and filling of shapes
	void EnableAntiAliasing(bool state);
	//! Enable PhysicsDebugRendering
	//void EnablePhysicsDebugRendering(bool state);


private:
	friend class GameEngine;
	// Not intended to be used by students
	GameSettings(void);
	// Not intended to be used by students
	String GetWindowTitle();
	// Not intended to be used by students
	int GetWindowWidth();
	// Not intended to be used by students
	int GetWindowHeight();
	// Not intended to be used by students
	bool IsConsoleEnabled();
	// Not intended to be used by students
	bool IsVSync();
	// Not intended to be used by students
	bool IsAntiAliasingEnabled();
	// Not intended to be used by students
	// bool IsDebugRenderingEnabled();


	String	m_WindowTitle = String("Game Engine");
	int		m_WindowWidth = 842;
	int		m_WindowHeight = 480;
	bool	m_bEnableConsole = false;
	bool	m_bVSync = true;
	bool	m_bEnableAntiAliasing = false;
	//bool	m_EnableDebugRendering = false;
};
