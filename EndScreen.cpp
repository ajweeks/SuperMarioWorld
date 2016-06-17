#include "stdafx.h"

#include "EndScreen.h"
#include "Game.h"
#include "Player.h"
#include "SpriteSheetManager.h"
#include "SMWFont.h"
#include "HUD.h"
#include "Camera.h"

const int EndScreen::FIRST_FADE_END = 0;
const int EndScreen::FIRST_FADE_END_VALUE = 140;
const int EndScreen::SECOND_FADE_BEGIN = 1;
const int EndScreen::SECOND_FADE_BEGIN_VALUE = 290;
const int EndScreen::SECOND_FADE_END = 2;
const int EndScreen::SECOND_FADE_END_VALUE = 70;
const int EndScreen::ENCLOSING_CIRCLE_END = 3;
const int EndScreen::ENCLOSING_CIRCLE_END_VALUE = 60;

const int EndScreen::DRUMROLL_START_POSITION = 120;

SMWMultiTimer EndScreen::m_EndScreenTransitionMultiTimer = SMWMultiTimer({});
Player* EndScreen::m_PlayerPtr;
Camera* EndScreen::m_CameraPtr;

int EndScreen::m_ScoreShowing;
int EndScreen::m_BonusScoreShowing;
int EndScreen::m_ExtraTime;

EndScreen::EndScreen()
{
}

EndScreen::~EndScreen()
{
}

void EndScreen::Initalize(Player* playerPtr, Camera* cameraPtr, int extraTime, int score, int bonusScore)
{
	m_PlayerPtr = playerPtr;
	m_CameraPtr = cameraPtr;
	m_ExtraTime = extraTime;
	m_ScoreShowing = score;
	m_BonusScoreShowing = bonusScore;

	// 1 fade from color to black
	// 2 play drumroll (no change in visuals at this mark)
	// 3 fade from black to color
	// 4 start shrinking black circle around player
	// 5 go back to the level select state

	m_EndScreenTransitionMultiTimer = SMWMultiTimer(
	{FIRST_FADE_END_VALUE, SECOND_FADE_BEGIN_VALUE, SECOND_FADE_END_VALUE, ENCLOSING_CIRCLE_END_VALUE });
}

bool EndScreen::Tick()
{
	const bool result = m_EndScreenTransitionMultiTimer.Tick();

	const int framesElapsed = m_EndScreenTransitionMultiTimer.FramesElapsedThisTimer();
	if (m_EndScreenTransitionMultiTimer.m_CurrentIndex == SECOND_FADE_BEGIN &&
		framesElapsed == DRUMROLL_START_POSITION)
	{
		SoundManager::PlaySoundEffect(SoundManager::Sound::DRUMROLL);
	}
	else if (m_EndScreenTransitionMultiTimer.m_CurrentIndex == ENCLOSING_CIRCLE_END &&
		framesElapsed == 0)
	{
		SoundManager::PlaySoundEffect(SoundManager::Sound::OUTRO_CIRCLE_TRANSITION);
	}

	return result;
}

void EndScreen::Paint()
{
	switch (m_EndScreenTransitionMultiTimer.m_CurrentIndex)
	{
	case FIRST_FADE_END: // We're in the first fade
	{
		// Fade from clear to black
		const int alpha = int(m_EndScreenTransitionMultiTimer.CurrentTimerPercentComplete() * 255);
		GAME_ENGINE->SetColor(COLOR(0, 0, 0, alpha));
		GAME_ENGINE->FillRect(0, 0, Game::WIDTH, Game::HEIGHT);
	} break;
	case SECOND_FADE_BEGIN: // Show solid black and text
	{
		GAME_ENGINE->SetColor(COLOR(0, 0, 0));
		GAME_ENGINE->FillRect(0, 0, Game::WIDTH, Game::HEIGHT);

		// Once the drumroll starts, the final score begins "moving" to the score in the top right
		if (m_EndScreenTransitionMultiTimer.CurrentTimerPercentComplete() > 0.45)
		{
			if (m_ScoreShowing > 0)
			{
				int delta = 10;
				if (m_ScoreShowing - delta < 0) delta = m_ScoreShowing;

				m_ScoreShowing -= delta;
				m_PlayerPtr->AddScore(delta, false);
			}

			if (m_BonusScoreShowing > 0)
			{
				m_BonusScoreShowing -= 1;
				m_PlayerPtr->AddRedStars(1);
			}
		}

		if (m_EndScreenTransitionMultiTimer.CurrentTimerPercentComplete() > 0.20)
		{
			PaintText();
		}
	} break;
	case SECOND_FADE_END: // Fade from black to color and paint text
	{
		const int alpha = int(255 - m_EndScreenTransitionMultiTimer.CurrentTimerPercentComplete() * 255);
		GAME_ENGINE->SetColor(COLOR(0, 0, 0, alpha));
		GAME_ENGINE->FillRect(0, 0, Game::WIDTH, Game::HEIGHT);

		PaintText();
	} break;
	case ENCLOSING_CIRCLE_END: // Show the enclosing circle and paint the text (underneath)
	{
		PaintText();

		const double percentage = 1.0 - m_EndScreenTransitionMultiTimer.CurrentTimerPercentComplete();
		const double innerCircleRadius = percentage * Game::WIDTH;
		DOUBLE2 playerPosScreenSpace = m_CameraPtr->GetViewMatrix().TransformPoint(m_PlayerPtr->GetPosition());
		PaintEnclosingCircle(playerPosScreenSpace, innerCircleRadius);
	} break;
	}
}

void EndScreen::PaintText()
{
	int left = 75;
	int top = 60;

	Bitmap* hudBmpPtr = SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::HUD);

	// MARIO
	RECT2 srcRect = RECT2(1, 1, 41, 9);
	GAME_ENGINE->DrawBitmap(hudBmpPtr, left + 32, top, srcRect);
	top += 16;

	// COURSE CLEAR
	GAME_ENGINE->SetColor(COLOR(255, 255, 255));
	SMWFont::PaintPhrase("COURSE CLEAR!", left, top, SMWFont::OUTLINED);
	top += 22;

	// Time
	SMWFont::PaintPhrase("@" + std::to_string(m_ExtraTime) + "*50=", left, top, SMWFont::OUTLINED);
	std::stringstream scoreShowingStream;
	scoreShowingStream << std::setw(5) << std::setfill(' ') << std::to_string(m_ScoreShowing);
	SMWFont::PaintPhrase(scoreShowingStream.str(), left + 65, top, SMWFont::OUTLINED);

	// Bonus red stars
	if (m_BonusScoreShowing > 0)
	{
		left += 4;
		top += 28;
		SMWFont::PaintPhrase("BONUS!", left, top, SMWFont::OUTLINED);

		// RED STAR
		left += 55;
		srcRect = RECT2(19, 60, 19 + 8, 60 + 8);
		GAME_ENGINE->DrawBitmap(hudBmpPtr, left, top, srcRect);

		// X
		left += 10;
		top += 2;
		srcRect = RECT2(10, 61, 10 + 7, 61 + 7);
		GAME_ENGINE->DrawBitmap(hudBmpPtr, left, top, srcRect);

		// NUMBER OF STARS
		left += 22;
		top -= 7;
		HUD::PaintSeveralDigitLargeNumber(left, top, m_BonusScoreShowing);
	}
}

// Paints everything black except a circle with the specified radius at the specified pos
void EndScreen::PaintEnclosingCircle(DOUBLE2 circleCenter, double innerCircleRadius)
{
	GAME_ENGINE->SetColor(COLOR(0, 0, 0));

	// Fill the largest four rectangles you can, and then draw several circles (just outlines)
	double circleLeft = circleCenter.x - innerCircleRadius;
	double circleRight = circleCenter.x + innerCircleRadius;
	double circleTop = circleCenter.y - innerCircleRadius;
	double circleBottom = circleCenter.y + innerCircleRadius;
	if (circleLeft > 1)
	{
		GAME_ENGINE->FillRect(0, 0, circleLeft, Game::HEIGHT);
	}
	if (circleRight < Game::WIDTH - 1)
	{
		GAME_ENGINE->FillRect(circleRight, 0, Game::WIDTH, Game::HEIGHT);
	}
	if (circleTop > 1)
	{
		GAME_ENGINE->FillRect(0, 0, Game::WIDTH, circleTop);
	}
	if (circleBottom < Game::HEIGHT - 1)
	{
		GAME_ENGINE->FillRect(0, circleBottom, Game::WIDTH, Game::HEIGHT);
	}

	int numOfCircles = 15;
	double radius = innerCircleRadius;
	for (int i = 0; i < numOfCircles; ++i)
	{
		GAME_ENGINE->DrawEllipse(circleCenter, radius, radius, 7);
		radius += 5;
	}
}
