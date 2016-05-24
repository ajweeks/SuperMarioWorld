#pragma once

#include "CountdownTimer.h"

class Level;

class Message
{
public:
	Message(std::string messageText, Level* levelPtr);
	virtual ~Message();

	Message(const Message&) = delete;
	Message& operator=(const Message&) = delete;

	void Tick(double deltaTime);
	void Paint();

	void StartIntroAnimation();
	void StartOutroAnimation();

	bool IsShowingMessage();
	bool IsInputPaused();

private:
	static const int WIDTH;
	static const int HEIGHT;

	CountdownTimer m_IntroAnimationTimer;
	CountdownTimer m_OutroAnimationTimer;

	std::string m_MessageText = "";
	Level* m_LevelPtr = nullptr;

	// This is true when the text of the message is showing, not during the intro/outro animations
	bool m_ShowingMessage = false;
};

