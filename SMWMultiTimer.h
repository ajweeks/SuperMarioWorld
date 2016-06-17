#pragma once

#include <vector>

// Represents multiple timers in sequence
struct SMWMultiTimer
{
	SMWMultiTimer(std::vector<int> timeStamps);

	// Returns true if the last timer is up
	bool Tick();
	int FramesElapsedThisTimer() const;
	double CurrentTimerPercentComplete() const; // Returns value in range [0.0, 1.0] representing the current timer's progression

	int m_CurrentIndex;
	int m_CurrentValue;
	std::vector<int> m_TimeStamps;
};