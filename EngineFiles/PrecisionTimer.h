//-----------------------------------------------------------------
// Game Engine
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

#pragma once

//=======================================================================================
// PrecisionTimer.h by Frank Luna (C) 2008 All Rights Reserved.
// Adapted for DAE by Bart Uyttenhove
//=======================================================================================
class PrecisionTimer
{
public:
	// -------------------------
	// Constructors & no Destructor -> use auto generated destructor and others
	// -------------------------
	PrecisionTimer();

	// -------------------------
	// General Methods
	// -------------------------

	// Returns the total time elapsed since reset() was called, NOT counting any
	// time when the clock is stopped.	
	double GetGameTime() const;  // in seconds

	// Returns the elapsed time since Tick was called
	double GetDeltaTime() const; // in seconds

	// Resets all 
	void Reset(); // Call before message loop.
	// Stores the start time
	void Start(); // Call when unpaused.
	// Stores the current time
	void Stop();  // Call when paused.
	// Calculates the time difference between this frame and the previous.
	// Use GetDeltaTime to retrieve that time
	void Tick();  // Call every frame.
	// Returns the state
	bool IsStopped() const;

private:
	//---------------------------
	// Private methods
	//---------------------------
	double m_SecondsPerCount = 0.0;
	double m_DeltaTime = -1.0;

	//---------------------------
	// Datamembers
	//---------------------------
	__int64 m_BaseTime = 0;
	__int64 m_PausedTime = 0;
	__int64 m_StopTime = 0;
	__int64 m_PrevTime = 0;
	__int64 m_CurrTime = 0;

	bool m_bStopped = false;
};