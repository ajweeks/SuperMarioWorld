#include "stdafx.h"

#include "Message.h"
#include "Game.h"
#include "Level.h"
#include "SMWFont.h"

const int Message::WIDTH = 160;
const int Message::HEIGHT = 78;

Message::Message(std::string messageText, Level* levelPtr) :
	m_LevelPtr(levelPtr),
	m_MessageText(messageText)
{
	m_IntroAnimationTimer = CountdownTimer(26);
	m_OutroAnimationTimer = CountdownTimer(26);
}

Message::~Message()
{
}

void Message::Tick(double deltaTime)
{
	if (m_IntroAnimationTimer.Tick() && m_IntroAnimationTimer.IsComplete())
	{
		m_ShowingMessage = true;
	}

	if (m_OutroAnimationTimer.Tick() && m_OutroAnimationTimer.IsComplete())
	{
		m_LevelPtr->SetActiveMessage(nullptr); // We are no longer the active message
		m_LevelPtr->SetPaused(false, false);
	}
}

void Message::Paint()
{
	MATRIX3X2 matViewPrevious = GAME_ENGINE->GetViewMatrix();
	GAME_ENGINE->SetViewMatrix(Game::matIdentity); // We want to paint in screen space, not world space
	GAME_ENGINE->SetColor(COLOR(0, 0, 0));

	int yo = 32;
	if (m_IntroAnimationTimer.IsActive())
	{
		// NOTE: Paints a growing black rectangle
		double framesElapsed = m_IntroAnimationTimer.FramesElapsed();
		double totalFrames = m_IntroAnimationTimer.OriginalNumberOfFrames();
		double width = double(framesElapsed) * (double(WIDTH) / double(totalFrames));
		double height = double(framesElapsed) * (double(HEIGHT) / double(totalFrames));

		double x = Game::WIDTH / 2 - width / 2;
		double y = Game::HEIGHT / 2 - height / 2 - yo;
		GAME_ENGINE->FillRect(RECT2(x, y, x + width, y + height));
	}
	else if (m_OutroAnimationTimer.IsActive())
	{
		// NOTE: Paints a shrinking black rectangle
		double framesElapsed = m_OutroAnimationTimer.FramesElapsed();
		double totalFrames = m_OutroAnimationTimer.OriginalNumberOfFrames();
		double width = WIDTH - double(framesElapsed) * (double(HEIGHT) / double(totalFrames));
		double height = HEIGHT - double(framesElapsed) * (double(HEIGHT) / double(totalFrames));

		double x = Game::WIDTH / 2 - width / 2;
		double y = Game::HEIGHT / 2 - height / 2 - yo;
		GAME_ENGINE->FillRect(RECT2(x, y, x + width, y + height));
	}
	else if (m_ShowingMessage)
	{
		double x = Game::WIDTH / 2 - WIDTH / 2;
		double y = Game::HEIGHT / 2 - HEIGHT / 2 - yo;
		GAME_ENGINE->FillRect(RECT2(x, y, x + WIDTH, y + HEIGHT));

		int xPad = 7;
		int yPad = 6;
		SMWFont::PaintPhrase(m_MessageText, x + xPad, y + yPad, false);
	}

	GAME_ENGINE->SetViewMatrix(matViewPrevious);
}

void Message::StartIntroAnimation()
{
	m_LevelPtr->SetActiveMessage(this);
	m_IntroAnimationTimer.Start();
}

void Message::StartOutroAnimation()
{
	if (m_ShowingMessage)
	{
		m_IntroAnimationTimer.SetComplete();
		m_OutroAnimationTimer.Start();
		m_ShowingMessage = false;
	}
}

bool Message::IsInputPaused()
{
	return m_ShowingMessage || m_IntroAnimationTimer.IsActive() || m_OutroAnimationTimer.IsActive();
}

bool Message::IsShowingMessage()
{
	return m_ShowingMessage;
}
