#pragma once

struct AnimationInfo
{
	double secondsPerFrame = 0.065;
	double secondsElapsedThisFrame = 0.0;
	int frameNumber = 0;

	//AnimationInfo(int totalFrames = 1, double secondsPerFrame = 0.065) :
	//	secondsPerFrame(secondsPerFrame),
	//	frameNumber(0),
	//	secondsElapsedThisFrame(0.0)
	//{
	//}

	//AnimationInfo(const AnimationInfo&) = delete;
	//AnimationInfo& operator=(const AnimationInfo&) = delete;

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
};
