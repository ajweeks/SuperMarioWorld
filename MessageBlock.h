#pragma once

#include "Block.h"

class Message;

class MessageBlock : public Block
{
public:
	MessageBlock(DOUBLE2 topLeft, std::string messageText, Level* levelPtr);
	virtual ~MessageBlock();

	MessageBlock(const MessageBlock&) = delete;
	MessageBlock& operator=(const MessageBlock&) = delete;

	void Tick(double deltaTime);
	void Paint();
	void Hit();

	bool PauseInput();
	bool ShowingMessage();
	void ClearShowingMessage();

private:
	Message* m_MessagePtr = nullptr;
	
	static const int BUMP_HEIGHT = 20;
	static const int FRAMES_OF_BUMP_ANIMATION = 12;

	CountdownTimer m_DelayBeforeIntroAnimationTimer;
	CountdownTimer m_BumpAnimationTimer;

	int m_yo = 0;
};
