#pragma once

#include "Block.h"

class MessageBlock : public Block
{
public:
	MessageBlock(DOUBLE2 topLeft, String bmpFilePath, Level* levelPtr);
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
	static int m_BitmapWidth;
	static int m_BitmapHeight;

	// LATER: Make message boxes render their text manually?
	//String m_Message;
	Bitmap* m_BmpPtr = nullptr;
	
	static const int BUMP_HEIGHT = 20;
	static const int FRAMES_OF_BUMP_ANIMATION = 12;

	CountdownTimer m_DelayBeforeIntroAnimationTimer;
	CountdownTimer m_IntroAnimationTimer;
	CountdownTimer m_OutroAnimationTimer;
	CountdownTimer m_BumpAnimationTimer;
	
	// This is true when the text of the message is showing, not during the intro/outro animations
	bool m_ShowingMessage = false; 
	int m_yo = 0;

};
