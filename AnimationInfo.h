#pragma once

struct AnimationInfo
{
	AnimationInfo() 
	{
		Reset();
	}

	AnimationInfo(const AnimationInfo&) = delete;
	AnimationInfo& operator=(const AnimationInfo&) = delete;

	void Tick(double deltaTime)
	{
		secondsElapsedThisFrame += deltaTime;
		if (secondsElapsedThisFrame > secondsPerFrame)
		{
			secondsElapsedThisFrame -= secondsPerFrame;
			++frameNumber;
		}
	}

	void Reset()
	{
		secondsElapsedThisFrame = 0.0;
		frameNumber = 0;
		secondsPerFrame = 0.065;
	}

	double secondsPerFrame;
	double secondsElapsedThisFrame;
	int frameNumber;
};
