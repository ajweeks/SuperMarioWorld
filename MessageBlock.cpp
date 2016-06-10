#include "stdafx.h"

#include "MessageBlock.h"
#include "Message.h"
#include "Game.h"
#include "Level.h"
#include "SoundManager.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

MessageBlock::MessageBlock(DOUBLE2 topLeft, std::string messageText, Level* levelPtr) :
	Block(topLeft, Type::MESSAGE_BLOCK, levelPtr)
{
	m_MessagePtr = new Message(messageText, levelPtr);

	m_DelayBeforeIntroAnimationTimer = SMWTimer(12);
	m_BumpAnimationTimer = SMWTimer(FRAMES_OF_BUMP_ANIMATION);
}

MessageBlock::~MessageBlock()
{
	delete m_MessagePtr;
}

void MessageBlock::Tick(double deltaTime)
{
	if (m_DelayBeforeIntroAnimationTimer.Tick() && m_DelayBeforeIntroAnimationTimer.IsComplete())
	{
		m_MessagePtr->StartIntroAnimation();
		return;
	}
	if (m_BumpAnimationTimer.Tick())
	{
		m_yo = m_BumpAnimationTimer.FramesElapsed();
		if (m_yo > FRAMES_OF_BUMP_ANIMATION / 2)
		{
			m_yo = (FRAMES_OF_BUMP_ANIMATION / 2) - (m_yo - (FRAMES_OF_BUMP_ANIMATION / 2));
		}
		m_yo = int((double(-m_yo) / FRAMES_OF_BUMP_ANIMATION) * BUMP_HEIGHT);

		if (m_BumpAnimationTimer.IsComplete())
		{
			m_yo = 0;
		}
	}
}

void MessageBlock::Paint()
{
	int srcCol = 5;
	int srcRow = 9;
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y + m_yo;
	SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES)->Paint(left, top, srcCol, srcRow);

	m_MessagePtr->Paint();
}

void MessageBlock::Hit()
{
	SoundManager::PlaySoundEffect(SoundManager::Sound::BLOCK_HIT);
	SoundManager::PlaySoundEffect(SoundManager::Sound::MESSAGE_BLOCK_HIT);

	m_DelayBeforeIntroAnimationTimer.Start();
	m_BumpAnimationTimer.Start();
	m_yo = 0;
}
