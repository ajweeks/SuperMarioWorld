#pragma once

enum class ActorId
{
	PLAYER, PLATFORM, PIPE, ENEMY, LEVEL, ITEM
};

enum class FACING_DIRECTION
{
	LEFT, RIGHT
};

struct ANIMATION_INFO
{
	double secondsElapsedThisFrame = 0.0;
	int frameNumber = 0;
	// TODO: Wait a minute, isn't deltaTime given in seconds? This should be secondsPerFrame, right?
	double secondsPerFrame = 0.065;

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

enum class COLOUR
{
	GREEN, YELLOW, BLUE, ORANGE, GREY
};

