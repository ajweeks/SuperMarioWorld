//-----------------------------------------------------------------
// Game Engine
// C++ Source - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------
#include "stdafx.h"
#include "../stdafx.h"
#include "PrecisionTimer.h"
#include "windows.h"

//=======================================================================================
// PrecisionTimer.cpp by Frank Luna (C) 2008 All Rights Reserved.
// Adapted for DAE by Bart Uyttenhove
//=======================================================================================
PrecisionTimer::PrecisionTimer()
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_SecondsPerCount = 1.0 / (double)countsPerSec;
}

// Returns the total time elapsed since reset() was called, NOT counting any
// time when the clock is stopped.
double PrecisionTimer::GetGameTime() const
{


	// If we are stopped, do not count the time that has passed since we stopped.
	//
	// ----*---------------*------------------------------*------> time
	//  mBaseTime       mStopTime                      mCurrTime

	if (m_bStopped)
	{
		return (double)(((m_StopTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
	}

	// The distance mCurrTime - mBaseTime includes paused time,
	// which we do not want to count.  To correct this, we can subtract 
	// the paused time from mCurrTime:  
	//
	//  (mCurrTime - mPausedTime) - mBaseTime 
	//
	//                     |<-------d------->|
	// ----*---------------*-----------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mCurrTime

	else
	{
		//Bart: Get current time
		QueryPerformanceCounter((LARGE_INTEGER*)&m_CurrTime);
		return (double)(((m_CurrTime - m_PausedTime) - m_BaseTime) * m_SecondsPerCount);
	}
}

double PrecisionTimer::GetDeltaTime() const
{
	return (double)m_DeltaTime;
}

void PrecisionTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_BaseTime = currTime;
	m_PrevTime = currTime;
	m_StopTime = 0;
	m_PausedTime = 0;
	m_bStopped = false;
}

void PrecisionTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);


	// Accumulate the time elapsed between stop and start pairs.
	//
	//                     |<-------d------->|
	// ----*---------------*-----------------*------------> time
	//  mBaseTime       mStopTime        startTime     

	if (m_bStopped)
	{
		m_PausedTime += (startTime - m_StopTime);
		m_StopTime = 0;
		m_bStopped = false;
	}
}

void PrecisionTimer::Stop()
{
	if (!m_bStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		m_StopTime = currTime;
		m_bStopped = true;
	}
}

void PrecisionTimer::Tick()
{
	if (m_bStopped)
	{
		m_DeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_CurrTime = currTime;

	// Time difference between this frame and the previous.
	m_DeltaTime = (m_CurrTime - m_PrevTime - m_PausedTime) * m_SecondsPerCount;

	// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
	// processor goes into a power save mode or we get shuffled to another
	// processor, then mDeltaTime can be negative.
	if (m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0;
	}

	// Prepare for next frame.
	m_PrevTime = m_CurrTime;
	m_PausedTime = 0;

}

bool PrecisionTimer::IsStopped() const
{
	return m_bStopped;
}