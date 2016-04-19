#pragma once

enum class ActorId
{
	PLAYER, PLATFORM, PIPE, ENEMY, LEVEL, ITEM
};

struct FacingDirection
{
	static const int LEFT = -1;
	static const int RIGHT = 1;

	static int OppositeDirection(int direction)
	{
		return (direction == RIGHT ? LEFT : RIGHT);
	}
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
	RED, GREEN, YELLOW, BLUE, ORANGE, PINK, GREY, NONE
};

class Colour
{
public:
	static std::string COLOURToString(COLOUR colour)
	{
		switch (colour)
		{
		case COLOUR::RED:
			return "Red";
		case COLOUR::GREEN:
			return "Green";
		case COLOUR::YELLOW:
			return "Yellow";
		case COLOUR::BLUE:
			return "Blue";
		case COLOUR::ORANGE:
			return "Orange";
		case COLOUR::PINK:
			return "Pink";
		case COLOUR::GREY:
			return "Grey";
		default:
			return "unknown colour";
		}
	}
	static COLOUR StringToCOLOUR(std::string str)
	{
		if (!str.compare("Red"))
		{
			return COLOUR::RED;
		}
		else if (!str.compare("Green"))
		{
			return COLOUR::GREEN;
		}
		else if (!str.compare("Yellow"))
		{
			return COLOUR::YELLOW;
		}
		else if (!str.compare("Blue"))
		{
			return COLOUR::BLUE;
		}
		else if (!str.compare("Orange"))
		{
			return COLOUR::ORANGE;
		}
		else if (!str.compare("Pink"))
		{
			return COLOUR::PINK;
		}
		else if (!str.compare("Grey"))
		{
			return COLOUR::GREY;
		}
		else
		{
			return COLOUR::NONE;
		}
	}
};

class FileIO
{
public:

	static std::string GetTagContent(std::string totalString, std::string tagString, int startPos = 0)
	{
		std::string result;

		int attributeBegin = totalString.find("<" + tagString + ">", startPos);
		if (attributeBegin != -1)
		{
			attributeBegin += std::string("<" + tagString + ">").length();
		}
		int attributeEnd = totalString.find("</" + tagString + ">", attributeBegin);
		if (attributeBegin == std::string::npos || attributeEnd == std::string::npos)
		{
			return "";
		}
		result = totalString.substr(attributeBegin, attributeEnd - attributeBegin);

		return result;
	}

	static RECT2 StringToRECT2(std::string string)
	{
		RECT2 result;

		int comma1 = string.find(",");
		int comma2 = string.find(",", comma1 + 1);
		int comma3 = string.find(",", comma2 + 1);

		result.left = stod(string.substr(0, comma1));
		result.top = stod(string.substr(comma1 + 1, comma2 - comma1 - 1));
		result.right = stod(string.substr(comma2 + 1, comma3 - comma2 - 1));
		result.bottom = stod(string.substr(comma3 + 1));

		return result;
	}
};