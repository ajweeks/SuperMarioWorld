#pragma once

struct AnimationInfo
{
	double secondsElapsedThisFrame = 0.0;
	int frameNumber = 0;
	// TODO: Wait a minute, isn't deltaTime given in seconds? This should be secondsPerFrame, right?
	double secondsPerFrame = 0.065;

	void Tick(double deltaTime);

	void Reset();
};
