#pragma once

enum class ActorId
{
	PLAYER, PLATFORM, PIPE, LEVEL, ITEM
};

enum class FACING_DIRECTION
{
	LEFT, RIGHT
};

enum class LOOKING_DIRECTION
{
	UP, MIDDLE, DOWN
};

struct ANIMATION_INFO
{
	double msThisFrame = 0.0;
	int frameNumber = 0;
	double msPerFrame = 0.065;

	void Tick(double deltaTime)
	{
		msThisFrame += deltaTime;
		if (msThisFrame > msPerFrame)
		{
			msThisFrame -= msPerFrame;
			++frameNumber;
		}
	}

	void Reset()
	{
		msThisFrame = 0.0;
		frameNumber = 0;
		msPerFrame = 0.065;
	}
};

enum class COLOUR
{
	GREEN, YELLOW, BLUE, ORANGE, GREY
};

