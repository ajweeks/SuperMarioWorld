#include "stdafx.h"

#include "SMWMultiTimer.h"

SMWMultiTimer::SMWMultiTimer(std::vector<int> timeStamps) :
	m_TimeStamps(timeStamps)
{
	m_CurrentIndex = 0;
	m_CurrentValue = 0;
}

// Returns true if the last timer is up
bool SMWMultiTimer::Tick()
{
	if (m_CurrentIndex >= int(m_TimeStamps.size())) return true;

	if (++m_CurrentValue > m_TimeStamps[m_CurrentIndex])
	{
		m_CurrentValue = 0;
		++m_CurrentIndex;
		if (m_CurrentIndex > int(m_TimeStamps.size()))
		{
			// We're done
			--m_CurrentValue;
			--m_CurrentIndex; // Just in case
			return true;
		}
	}
	return false;
}

double SMWMultiTimer::CurrentTimerPercentComplete() const
{
	double percent = double(m_CurrentValue) / double(m_TimeStamps[m_CurrentIndex]);

	return percent;
}

int SMWMultiTimer::FramesElapsedThisTimer() const
{
	if (m_CurrentIndex >= int(m_TimeStamps.size())) return -1;

	return m_CurrentValue;
}
