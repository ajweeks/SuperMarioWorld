#include "stdafx.h"

#include "CountdownTimer.h"

CountdownTimer::CountdownTimer() : 
	CountdownTimer(-1) 
{
}

CountdownTimer::CountdownTimer(int numberOfFrames) :
	TOTAL_FRAMES(numberOfFrames)
{
	m_FramesRemaining = -1;
	m_IsActive = false;
}

void CountdownTimer::Start()
{
	m_FramesRemaining = TOTAL_FRAMES;
	m_IsActive = true;
}

bool CountdownTimer::Tick()
{
	if (m_IsActive)
	{
		--m_FramesRemaining;
		if (m_FramesRemaining <= 0)
		{
			m_IsActive = false;
			m_FramesRemaining = 0;
			return true;
		}

		return m_IsActive;
	}
	else
	{
		return false;
	}
}

bool CountdownTimer::IsComplete()
{
	return m_FramesRemaining == 0;
}

bool CountdownTimer::IsActive()
{
	return m_IsActive;
}

int CountdownTimer::FramesElapsed()
{
	return (TOTAL_FRAMES - m_FramesRemaining);
}

int CountdownTimer::OriginalNumberOfFrames()
{
	return TOTAL_FRAMES;
}

void CountdownTimer::SetComplete()
{
	m_FramesRemaining = 0;
	m_IsActive = false;
}
