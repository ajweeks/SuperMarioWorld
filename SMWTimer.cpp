#include "stdafx.h"

#include "SMWTimer.h"

SMWTimer::SMWTimer() : 
	SMWTimer(-1) 
{
}

SMWTimer::SMWTimer(int numberOfFrames) :
	TOTAL_FRAMES(numberOfFrames)
{
	m_FramesRemaining = -1;
	m_IsActive = false;
	m_IsPaused = false;
}

void SMWTimer::Start()
{
	m_FramesRemaining = TOTAL_FRAMES;
	m_IsActive = true;
	if (m_IsPaused) m_IsPaused = false;
}

bool SMWTimer::Tick()
{
	if (m_IsActive && m_IsPaused == false)
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

bool SMWTimer::IsComplete() const
{
	return m_FramesRemaining == 0;
}

bool SMWTimer::IsActive() const
{
	return m_IsActive;
}

int SMWTimer::FramesElapsed() const
{
	return (TOTAL_FRAMES - m_FramesRemaining);
}

int SMWTimer::TotalFrames() const
{
	return TOTAL_FRAMES;
}

void SMWTimer::SetComplete()
{
	m_FramesRemaining = 0;
	m_IsActive = false;
	m_IsPaused = false;
}

void SMWTimer::SetPaused(bool paused)
{
	m_IsPaused = paused;
}

bool SMWTimer::IsPaused() const
{
	return m_IsPaused;
}