#include "stdafx.h"

#include "MessageBlock.h"
#include "Game.h"
#include "Level.h"
#include "SoundManager.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

int MessageBlock::m_BitmapWidth;
int MessageBlock::m_BitmapHeight;

MessageBlock::MessageBlock(DOUBLE2 topLeft, String bmpFilePath, Level* levelPtr) :
	Block(topLeft, Type::MESSAGE_BLOCK, levelPtr)
{
	m_BmpPtr = new Bitmap(bmpFilePath);
	m_BitmapWidth = m_BmpPtr->GetWidth();
	m_BitmapHeight = m_BmpPtr->GetHeight();
	m_AnimInfo.frameNumber = 0;

	m_DelayBeforeIntroAnimationTimer = CountdownTimer(12);
	m_IntroAnimationTimer = CountdownTimer(26);
	m_OutroAnimationTimer = CountdownTimer(26);
	m_BumpAnimationTimer = CountdownTimer(FRAMES_OF_BUMP_ANIMATION);
}

MessageBlock::~MessageBlock()
{
	delete m_BmpPtr;
}

void MessageBlock::Tick(double deltaTime)
{
	if (m_DelayBeforeIntroAnimationTimer.Tick() && m_DelayBeforeIntroAnimationTimer.IsComplete())
	{
		m_IntroAnimationTimer.Start();
		return;
	}

	if (m_IntroAnimationTimer.Tick() && m_IntroAnimationTimer.IsComplete())
	{
		m_ShowingMessage = true;
	}

	if (m_OutroAnimationTimer.Tick() && m_OutroAnimationTimer.IsComplete())
	{
		m_LevelPtr->SetActiveMessageBlock(nullptr); // We are no longer the active message block
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
	SpriteSheetManager::generalTilesPtr->Paint(left, top, srcCol, srcRow);

	int yo = 32;
	GAME_ENGINE->SetColor(COLOR(0, 0, 0));
	MATRIX3X2 matViewPrevious = GAME_ENGINE->GetViewMatrix();
	GAME_ENGINE->SetViewMatrix(Game::matIdentity);

	if (m_IntroAnimationTimer.IsActive())
	{
		// NOTE: Paints a growing black rectangle
		double framesElapsed = m_IntroAnimationTimer.FramesElapsed();
		double totalFrames = m_IntroAnimationTimer.OriginalNumberOfFrames();
		double width = double(framesElapsed) * (double(m_BitmapWidth) / double(totalFrames));
		double height = double(framesElapsed) * (double(m_BitmapHeight) / double(totalFrames));
		
		double x = Game::WIDTH / 2 - width / 2;
		double y = Game::HEIGHT / 2 - height / 2 - yo;
		GAME_ENGINE->FillRect(RECT2(x, y, x + width, y + height));
	}
	else if (m_OutroAnimationTimer.IsActive())
	{
		// NOTE: Paints a shrinking black rectangle
		double framesElapsed = m_OutroAnimationTimer.FramesElapsed();
		double totalFrames = m_OutroAnimationTimer.OriginalNumberOfFrames();
		double width = m_BitmapWidth - double(framesElapsed) * (double(m_BitmapWidth) / double(totalFrames));
		double height = m_BitmapHeight - double(framesElapsed) * (double(m_BitmapHeight) / double(totalFrames));

		double x = Game::WIDTH / 2 - width / 2;
		double y = Game::HEIGHT / 2 - height / 2 - yo;
		GAME_ENGINE->FillRect(RECT2(x, y, x + width, y + height));
	}
	else if (m_ShowingMessage)
	{
		// NOTE: Paints a static black rectangle
		double x = Game::WIDTH / 2 - m_BmpPtr->GetWidth() / 2;
		double y = 25;
		GAME_ENGINE->DrawBitmap(m_BmpPtr, x, Game::HEIGHT / 2 - m_BmpPtr->GetHeight() / 2 - yo);
	}

	GAME_ENGINE->SetViewMatrix(matViewPrevious);
}

void MessageBlock::Hit()
{
	SoundManager::PlaySoundEffect(SoundManager::Sound::BLOCK_HIT);
	SoundManager::PlaySoundEffect(SoundManager::Sound::MESSAGE_BLOCK_HIT);

	m_LevelPtr->SetActiveMessageBlock(this);
	m_DelayBeforeIntroAnimationTimer.Start();
	m_BumpAnimationTimer.Start();
	m_yo = 0;
}

bool MessageBlock::PauseInput()
{
	return m_ShowingMessage || m_IntroAnimationTimer.IsActive() || m_OutroAnimationTimer.IsActive();
}

bool MessageBlock::ShowingMessage()
{
	return m_ShowingMessage;
}

void MessageBlock::ClearShowingMessage()
{
	if (m_ShowingMessage)
	{
		m_OutroAnimationTimer.Start();
		m_ShowingMessage = false;
	}
}
