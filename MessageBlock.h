#pragma once

#include "Block.h"

class MessageBlock : public Block
{
public:
	MessageBlock(DOUBLE2 topLeft, String bmpFilePath, Level* levelPtr);
	virtual ~MessageBlock();
	void Tick(double deltaTime);
	void Paint();
	void Hit(Level* levelPtr);

private:
	static int m_BitmapWidth;
	static int m_BitmapHeight;

	// LATER: Make message boxes render their text manually?
	//String m_Message;
	Bitmap* m_BmpPtr = nullptr;
	// NOTE: In the original game, after hitting a message block, the game doesn't pause for a second or so
	int m_FramesUntilPause = -1;
	static const int FRAMES_TO_WAIT = 12;
	// NOTE: Used to animate message popup, when == -1 there is no message showing
	// When != -1 the game is paused and the message is being painted (by the message block itself)
	int m_FramesOfIntroAnimation = -1;
	int m_FramesOfOutroAnimation = -1;
	static const int FRAMES_OF_ANIMATION;

	int m_CurrentFrameOfBumpAnimation = -1;
	static const int FRAMES_OF_BUMP_ANIMATION = 12;
	static const int BUMP_HEIGHT = 20;
	int m_yo = 0;

};
