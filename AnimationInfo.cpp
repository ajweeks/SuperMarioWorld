#include "stdafx.h"

#include "AnimationInfo.h"

void AnimationInfo::Tick(double deltaTime)
{
	secondsElapsedThisFrame += deltaTime;
	if (secondsElapsedThisFrame > secondsPerFrame)
	{
		secondsElapsedThisFrame -= secondsPerFrame;
		++frameNumber;
	}
}

void AnimationInfo::Reset()
{
	secondsElapsedThisFrame = 0.0;
	frameNumber = 0;
	secondsPerFrame = 0.065;
}