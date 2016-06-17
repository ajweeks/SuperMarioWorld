#pragma once

struct SMWTimer
{
	SMWTimer();
	SMWTimer(int numberOfFrames);

	void Start();
	bool IsActive() const;

	// Returns true if this timer is active, or if it just completed
	bool Tick();

	int FramesRemaining() const;
	int FramesElapsed() const;
	int TotalFrames() const;
	void SetFramesRemaining(int framesRemaining);

	void SetPaused(bool paused);
	bool IsPaused() const;

	void SetComplete();
	bool IsComplete() const;

private:
	int TOTAL_FRAMES;
	int m_FramesRemaining;
	bool m_IsActive;
	bool m_IsPaused;
};
