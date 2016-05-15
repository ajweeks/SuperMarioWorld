#pragma once

struct CountdownTimer
{
	CountdownTimer();
	CountdownTimer(int numberOfFrames);

	void Start();

	// Returns true if this timer is active, or if it just completed
	bool Tick();

	bool IsComplete() const;
	bool IsActive() const;
	int FramesElapsed() const;
	int OriginalNumberOfFrames() const;

	void SetComplete();

private:
	int TOTAL_FRAMES;
	int m_FramesRemaining;
	bool m_IsActive;
};
