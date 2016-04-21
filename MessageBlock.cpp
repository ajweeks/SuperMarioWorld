#include "stdafx.h"

#include "MessageBlock.h"
#include "Game.h"
#include "Level.h"
#include "SoundManager.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

int MessageBlock::m_BitmapWidth = -1;
int MessageBlock::m_BitmapHeight = -1;
const int MessageBlock::FRAMES_OF_ANIMATION = 360;

MessageBlock::MessageBlock(DOUBLE2 topLeft, String bmpFilePath, Level* levelPtr) :
	Block(topLeft, TYPE::MESSAGE_BLOCK, levelPtr)
{
	m_BmpPtr = new Bitmap(bmpFilePath);
	m_BitmapWidth = m_BmpPtr->GetWidth();
	m_BitmapHeight = m_BmpPtr->GetHeight();
	m_AnimInfo.frameNumber = 0;
}

MessageBlock::~MessageBlock()
{
	delete m_BmpPtr;
}

void MessageBlock::Tick(double deltaTime)
{
	if (m_FramesUntilPause != -1 && --m_FramesUntilPause <= 0)
	{
		m_LevelPtr->SetShowingMessage(true);
		m_FramesUntilPause = -1;
		m_FramesOfIntroAnimation = 0;
	}

	if (m_FramesOfIntroAnimation > -1)
	{
		if (m_LevelPtr->IsShowingMessage() == false)
		{
			m_FramesOfIntroAnimation = -1;
			m_FramesOfOutroAnimation = 0;
		}
	}

	if (m_FramesOfOutroAnimation > FRAMES_OF_ANIMATION)
	{
		m_FramesOfOutroAnimation = -1;
	}
}

void MessageBlock::Paint()
{
	int srcCol = 5;
	int srcRow = 9;
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	SpriteSheetManager::generalTilesPtr->Paint(left, top, srcCol, srcRow);

	int yo = 32;
	GAME_ENGINE->SetColor(COLOR(0, 0, 0));
	MATRIX3X2 matViewPrevious = GAME_ENGINE->GetViewMatrix();
	GAME_ENGINE->SetViewMatrix(Game::matIdentity);

	if (m_FramesOfIntroAnimation > -1)
	{
		m_FramesOfIntroAnimation++;

		// NOTE: Paints a growing rectangle for the first 'FRAMES_OF_ANIMATION' frames
		if (m_FramesOfIntroAnimation < FRAMES_OF_ANIMATION)
		{
			double width = double(m_FramesOfIntroAnimation) * (double(m_BitmapWidth) / double(FRAMES_OF_ANIMATION));
			double height = double(m_FramesOfIntroAnimation) * (double(m_BitmapHeight) / double(FRAMES_OF_ANIMATION));
			double x = Game::WIDTH / 2 - width / 2;
			double y = Game::HEIGHT / 2 - height / 2 - yo;
			GAME_ENGINE->FillRect(RECT2(x, y, x + width, y + height));
		}
		else
		{
			double x = Game::WIDTH / 2 - m_BmpPtr->GetWidth() / 2;
			double y = 25;
			GAME_ENGINE->DrawBitmap(m_BmpPtr, x, Game::HEIGHT / 2 - m_BmpPtr->GetHeight() / 2 - yo);
		}
	}
	else if (m_FramesOfOutroAnimation > -1)
	{
		m_FramesOfOutroAnimation++;

		// NOTE: Paints a shrinking rectangle for 'FRAMES_OF_ANIMATION' frames
		if (m_FramesOfOutroAnimation < FRAMES_OF_ANIMATION)
		{
			double width = m_BitmapWidth - double(m_FramesOfOutroAnimation) * (double(m_BitmapWidth) / double(FRAMES_OF_ANIMATION));
			double height = m_BitmapHeight - double(m_FramesOfOutroAnimation) * (double(m_BitmapHeight) / double(FRAMES_OF_ANIMATION));
			double x = Game::WIDTH / 2 - width / 2;
			double y = Game::HEIGHT / 2 - height / 2 - yo;
			GAME_ENGINE->FillRect(RECT2(x, y, x + width, y + height));
		}
	}

	GAME_ENGINE->SetViewMatrix(matViewPrevious);
}

void MessageBlock::Hit(Level* levelPtr)
{
	SoundManager::PlaySoundEffect(SoundManager::SOUND::BLOCK_HIT);

	m_FramesUntilPause = FRAMES_TO_WAIT;

	SoundManager::PlaySoundEffect(SoundManager::SOUND::MESSAGE_BLOCK_HIT);
}
