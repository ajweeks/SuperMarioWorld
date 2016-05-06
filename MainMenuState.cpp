#include "stdafx.h"

#include "MainMenuState.h"
#include "Game.h"
#include "StateManager.h"
#include "GameState.h"
#include "SoundManager.h"
#include "SpriteSheetManager.h"
#include "SMWFont.h"

MainMenuState::MainMenuState(StateManager* stateManagerPtr) :
	BaseState(stateManagerPtr, STATE_TYPE::MAIN_MENU)
{
	m_ScreenShowing = SCREEN::START;

	SoundManager::PlaySong(SoundManager::SONG::MENU_SCREEN_BGM);

	m_IntroTitleTimer = CountdownTimer(140);
	m_IntroFadeInTimer = CountdownTimer(75);
	m_OutroFadeOutTimer = CountdownTimer(75);

	m_IntroTitleTimer.Start();
}

MainMenuState::~MainMenuState()
{
}

void MainMenuState::Tick(double deltaTime)
{
	if (m_IntroTitleTimer.Tick() && m_IntroTitleTimer.IsComplete())
	{
		m_IntroFadeInTimer.Start();
	}

	if (m_IntroFadeInTimer.Tick())
	{
		if (GAME_ENGINE->IsKeyboardKeyPressed(VK_SPACE) ||
			GAME_ENGINE->IsKeyboardKeyPressed('A') ||
			GAME_ENGINE->IsKeyboardKeyPressed('S') ||
			GAME_ENGINE->IsKeyboardKeyPressed('Z') ||
			GAME_ENGINE->IsKeyboardKeyPressed('X'))
		{
			m_IntroFadeInTimer.SetComplete();
		}
		return;
	}

	if (m_OutroFadeOutTimer.Tick() && m_OutroFadeOutTimer.IsComplete())
	{
		m_StateManagerPtr->PushState(new GameState(m_StateManagerPtr));
		return;
	}

	if (m_IntroTitleTimer.IsActive() || m_OutroFadeOutTimer.IsActive()) 
		return;

	m_CursorAnimationTimer++;
	m_CursorAnimationTimer %= MAX_CURSOR_TIMER_VALUE;

	switch (m_ScreenShowing)
	{
	case SCREEN::START:
	{
		if (GAME_ENGINE->IsKeyboardKeyPressed(VK_SPACE) ||
			GAME_ENGINE->IsKeyboardKeyPressed('A') ||
			GAME_ENGINE->IsKeyboardKeyPressed('S') ||
			GAME_ENGINE->IsKeyboardKeyPressed('Z') ||
			GAME_ENGINE->IsKeyboardKeyPressed('X'))
		{
			m_ScreenShowing = SCREEN::SAVE_SELECTION;
			m_CursorIndex = 0;
			m_MaxCursorIndex = 3;
		}
	} break;
	case SCREEN::SAVE_SELECTION:
	{
		if (GAME_ENGINE->IsKeyboardKeyPressed(VK_DOWN))
		{
			++m_CursorIndex;
			if (m_CursorIndex > m_MaxCursorIndex) m_CursorIndex = 0;

			SoundManager::PlaySoundEffect(SoundManager::SOUND::FIRE_BALL_THROW);
			m_CursorAnimationTimer = 0;
		}
		else if (GAME_ENGINE->IsKeyboardKeyPressed(VK_UP))
		{
			--m_CursorIndex;			
			if (m_CursorIndex < 0) m_CursorIndex = m_MaxCursorIndex;
		
			SoundManager::PlaySoundEffect(SoundManager::SOUND::FIRE_BALL_THROW);
			m_CursorAnimationTimer = 0;
		}
		else if (GAME_ENGINE->IsKeyboardKeyPressed(VK_SPACE) ||
			GAME_ENGINE->IsKeyboardKeyPressed('Z') ||
			GAME_ENGINE->IsKeyboardKeyPressed('X'))
		{
			// TODO: Actually use the selected save state here

			m_ScreenShowing = SCREEN::PARTY_SIZE;
			m_CursorIndex = 0;
			m_MaxCursorIndex = 1;
			m_CursorAnimationTimer = 0;

			SoundManager::PlaySoundEffect(SoundManager::SOUND::COIN_COLLECT);
		}
	} break;
	case SCREEN::PARTY_SIZE:
	{
		if (GAME_ENGINE->IsKeyboardKeyPressed(VK_DOWN))
		{
			++m_CursorIndex;
			if (m_CursorIndex > m_MaxCursorIndex) m_CursorIndex = 0;

			SoundManager::PlaySoundEffect(SoundManager::SOUND::FIRE_BALL_THROW);
			m_CursorAnimationTimer = 0;
		}
		else if (GAME_ENGINE->IsKeyboardKeyPressed(VK_UP))
		{
			--m_CursorIndex;
			if (m_CursorIndex < 0) m_CursorIndex = m_MaxCursorIndex;

			SoundManager::PlaySoundEffect(SoundManager::SOUND::FIRE_BALL_THROW);
			m_CursorAnimationTimer = 0;
		}
		else if (GAME_ENGINE->IsKeyboardKeyPressed(VK_SPACE) ||
			GAME_ENGINE->IsKeyboardKeyPressed('Z') ||
			GAME_ENGINE->IsKeyboardKeyPressed('X'))
		{
			m_OutroFadeOutTimer.Start();
			SoundManager::PlaySoundEffect(SoundManager::SOUND::COIN_COLLECT);
		}
		else if (GAME_ENGINE->IsKeyboardKeyPressed('A') ||
				 GAME_ENGINE->IsKeyboardKeyPressed('S'))
		{	
			m_ScreenShowing = SCREEN::SAVE_SELECTION;
			m_CursorIndex = 0;
			m_MaxCursorIndex = 3;
			m_CursorAnimationTimer = 0;
		}
	} break;
	default:
		OutputDebugString(String("ERROR: Unhandled main menu state!\n"));
	}
}

void MainMenuState::Paint()
{
	if (m_IntroTitleTimer.IsActive())
	{
		GAME_ENGINE->SetColor(COLOR(0, 0, 0));
		GAME_ENGINE->FillRect(0, 0, Game::WIDTH, Game::HEIGHT);

		return;
	}

	GAME_ENGINE->SetColor(COLOR(35, 35, 35));
	GAME_ENGINE->FillRect(0, 0, Game::WIDTH, Game::HEIGHT);


	PaintBorder();

	// Main title
	int left = 25, top = 38;
	RECT2 srcRect = RECT2(0, 0, 208, 66);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::mainMenuScreenPtr, left, top, srcRect);

	// Copyright
	left = 55;
	top += 156;
	srcRect = RECT2(208, 58, 359, 66);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::mainMenuScreenPtr, left, top, srcRect);

	int lineHeight = 16;
	
	switch (m_ScreenShowing)
	{
	case SCREEN::START:
	{
	} break;
	case SCREEN::SAVE_SELECTION:
	{
		left = 80;
		top = 125;
		SMWFont::PaintPhrase("MARIO A ... 2", left, top, true);
		top += lineHeight;
		SMWFont::PaintPhrase("MARIO B ...EMPTY", left, top, true);
		top += lineHeight;
		SMWFont::PaintPhrase("MARIO C ...EMPTY", left, top, true);
		top += lineHeight;
		SMWFont::PaintPhrase("ERASE DATA", left, top, true);

		left -= 16;
		top = 125 + m_CursorIndex * lineHeight;
		PaintCursor(left, top);
	} break;
	case SCREEN::PARTY_SIZE:
	{
		left = 85;
		top = 128;
		SMWFont::PaintPhrase("1 PLAYER GAME", left, top, true);
		top += lineHeight;
		SMWFont::PaintPhrase("2 PLAYER GAME", left, top, true);

		left -= 16;
		top = 128 + m_CursorIndex * lineHeight;
		PaintCursor(left, top);
	} break;
	default:
		OutputDebugString(String("ERROR: Unhandled main menu state!\n"));
	}

	if (m_IntroFadeInTimer.IsActive())
	{
		int alpha = int(255 - (double(m_IntroFadeInTimer.FramesElapsed()) / double(m_IntroFadeInTimer.OriginalNumberOfFrames())) * 255);
		GAME_ENGINE->SetColor(COLOR(0, 0, 0, alpha));
		GAME_ENGINE->FillRect(0, 0, Game::WIDTH, Game::HEIGHT);
	}
	else if (m_OutroFadeOutTimer.IsActive())
	{
		int alpha = int((double(m_OutroFadeOutTimer.FramesElapsed()) / double(m_OutroFadeOutTimer.OriginalNumberOfFrames())) * 255);
		GAME_ENGINE->SetColor(COLOR(0, 0, 0, alpha));
		GAME_ENGINE->FillRect(0, 0, Game::WIDTH, Game::HEIGHT);
	}
}

void MainMenuState::PaintCursor(int left, int top)
{
	if (m_CursorAnimationTimer > MAX_CURSOR_TIMER_VALUE - 8)
	{
		return;
	}

	RECT2 srcRect = RECT2(285, 5, 285 + 8, 5 + 8);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::mainMenuScreenPtr, left, top, srcRect);
}

void MainMenuState::PaintBorder()
{
	// TOP
	int left = 0, top = 0;
	DrawBorderPlank(SHORT_PLANK, left, top);

	left += 32;
	DrawBorderPlank(LONG_PLANK, left, top, true);

	left += 104;
	DrawBorderPlank(LONG_PLANK, left, top, true);

	left += 104;
	DrawBorderPlank(LONG_PLANK, left, top);

	// BOTTOM
	left = -32;
	top = Game::HEIGHT - 16;
	DrawBorderPlank(LONG_PLANK, left, top);

	left += 104;
	DrawBorderPlank(LONG_PLANK, left, top);

	left += 104;
	DrawBorderPlank(MED_PLANK, left, top);

	// LEFT
	left = -16;
	top = 16;
	for (int i = 0; i < 6; ++i)
	{
		DrawBorderPlank(SHORT_PLANK, left, top);
		top += 16;
		DrawBorderPlank(MED_PLANK, left - 64, top);
		top += 16;
	}

	// RIGHT
	left = Game::WIDTH - 16;
	top = 16;
	for (int i = 0; i < 6; ++i)
	{
		DrawBorderPlank(MED_PLANK, left, top, true);
		top += 16;
		DrawBorderPlank(MED_PLANK, left, top, false, true);
		top += 16;
	}

}

void MainMenuState::DrawBorderPlank(int plankIndex, int left, int top, bool flipV, bool flipH)
{
	assert(plankIndex >= 0 && plankIndex <= 2);

	MATRIX3X2 matPrevWorld = GAME_ENGINE->GetWorldMatrix();

	RECT2 srcRect;
	switch (plankIndex)
	{
	case SHORT_PLANK:
		srcRect = RECT2(212, 19, 212 + 32, 19 + 16);
		break;
	case LONG_PLANK:
		srcRect = RECT2(244, 19, 244 + 104, 19 + 16);
		break;
	case MED_PLANK:
		srcRect = RECT2(244, 35, 244 + 96, 35 + 16);
		break;
	}

	if (flipV || flipH)
	{
		int halfWidth = int((srcRect.right - srcRect.left) / 2);
		int halfHeight = int((srcRect.bottom - srcRect.top) / 2);
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(left + halfWidth, top + halfHeight);
		int xScale = (flipH ? -1 : 1);
		int yScale = (flipV ? -1 : 1);
		MATRIX3X2 matRefelct = MATRIX3X2::CreateScalingMatrix(xScale, yScale);
		MATRIX3X2 matTranslateInverse = MATRIX3X2::CreateTranslationMatrix(-(left + halfWidth), -(top + halfHeight));
		MATRIX3X2 matTotal = matTranslateInverse * matRefelct * matTranslate;

		GAME_ENGINE->SetWorldMatrix(matTotal);
	}
	
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::mainMenuScreenPtr, left, top, srcRect);

	GAME_ENGINE->SetWorldMatrix(matPrevWorld);
}