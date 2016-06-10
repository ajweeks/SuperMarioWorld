#include "stdafx.h"

#include "MainMenuState.h"
#include "Game.h"
#include "StateManager.h"
#include "GameState.h"
#include "SoundManager.h"
#include "SpriteSheetManager.h"
#include "SMWFont.h"
#include "Keybindings.h"

MainMenuState::MainMenuState(StateManager* stateManagerPtr) :
	BaseState(stateManagerPtr, StateType::MAIN_MENU)
{
	EnterScreen(Screen::START);

	SoundManager::PlaySong(SoundManager::Song::MENU_SCREEN_BGM);

	m_IntroTitleTimer = SMWTimer(190);
	m_IntroFadeInTimer = SMWTimer(75);
	m_OutroFadeOutTimer = SMWTimer(75);

	m_IntroTitleTimer.Start();

	SoundManager::PlaySoundEffect(SoundManager::Sound::COIN_COLLECT);
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
		if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::START_BUTTON) ||
			GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::Y_BUTTON) ||
			GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::X_BUTTON) ||
			GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::A_BUTTON) ||
			GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::B_BUTTON))
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

	// All states but start state handle up/down presses identically
	if (m_ScreenShowing != Screen::START)
	{
		if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::D_PAD_DOWN))
		{
			++m_CursorIndex;
			if (m_CursorIndex > m_MaxCursorIndex) m_CursorIndex = 0;

			SoundManager::PlaySoundEffect(SoundManager::Sound::FIRE_BALL_THROW);
			m_CursorAnimationTimer = 0;
		}
		else if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::D_PAD_UP))
		{
			--m_CursorIndex;
			if (m_CursorIndex < 0) m_CursorIndex = m_MaxCursorIndex;

			SoundManager::PlaySoundEffect(SoundManager::Sound::FIRE_BALL_THROW);
			m_CursorAnimationTimer = 0;
		}
	}
	
	switch (m_ScreenShowing)
	{
	case Screen::START:
	{
		if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::START_BUTTON) ||
			GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::Y_BUTTON) ||
			GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::X_BUTTON) ||
			GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::A_BUTTON) ||
			GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::B_BUTTON))
		{
			EnterScreen(Screen::SAVE_SELECTION);
		}
	} break;
	case Screen::SAVE_SELECTION:
	{
		if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::START_BUTTON) ||
			GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::A_BUTTON) ||
			GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::B_BUTTON))
		{
			if (m_CursorIndex == 3)
			{
				EnterScreen(Screen::ERASE_DATA);	
			}
			else
			{
				// TODO: Actually use the selected save state here

				EnterScreen(Screen::PARTY_SIZE);
			}

			SoundManager::PlaySoundEffect(SoundManager::Sound::COIN_COLLECT);
		}
	} break;
	case Screen::ERASE_DATA:
	{
		if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::START_BUTTON) ||
			GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::A_BUTTON) ||
			GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::B_BUTTON))
		{
			if (m_CursorIndex == 3)
			{
				EnterScreen(Screen::START);
				m_IntroFadeInTimer.Start();
			}
			else
			{
				// LATER: Actually erase the data here
			}

			SoundManager::PlaySoundEffect(SoundManager::Sound::COIN_COLLECT);
		}
		else if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::Y_BUTTON) ||
			GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::X_BUTTON))
		{
			EnterScreen(Screen::SAVE_SELECTION);
		}
	} break;
	case Screen::PARTY_SIZE:
	{
		if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::START_BUTTON) ||
			GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::A_BUTTON) ||
			GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::B_BUTTON))
		{
			m_OutroFadeOutTimer.Start();
			SoundManager::PlaySoundEffect(SoundManager::Sound::COIN_COLLECT);
		}
		else if (GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::Y_BUTTON) ||
				 GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::X_BUTTON))
		{	
			EnterScreen(Screen::SAVE_SELECTION);
		}
	} break;
	default:
		OutputDebugString(String("ERROR: Unhandled main menu state!\n"));
	}
}

void MainMenuState::EnterScreen(Screen newScreen)
{
	m_ScreenShowing = newScreen;
	switch (newScreen)
	{
	case Screen::START:
	{
	} break;
	case Screen::SAVE_SELECTION:
	{
		m_CursorIndex = 0;
		m_MaxCursorIndex = 3;
		m_CursorAnimationTimer = 0;
	} break;
	case Screen::PARTY_SIZE:
	{
		m_CursorIndex = 0;
		m_MaxCursorIndex = 1;
		m_CursorAnimationTimer = 0;
	} break;
	case Screen::ERASE_DATA:
	{
		m_CursorIndex = 0;
		m_CursorAnimationTimer = 0;
		m_MaxCursorIndex = 3;
	} break;
	}
}

void MainMenuState::Paint()
{
	if (m_IntroTitleTimer.IsActive())
	{
		GAME_ENGINE->SetColor(COLOR(0, 0, 0));
		GAME_ENGINE->FillRect(0, 0, Game::WIDTH, Game::HEIGHT);

		// - Nintendo Presentes -
		int left = Game::WIDTH / 2 - 32;
		int top = Game::HEIGHT / 2 - 8;
		RECT2 srcRect = RECT2(212, 1, 212 + 64, 1 + 16);
		GAME_ENGINE->DrawBitmap(SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::MAIN_MENU_SCREEN), left, top, srcRect);
		
		int fadeStartTime = 85;
		int fadeEndTime = 125;
		// Fade to Black
		if (m_IntroTitleTimer.FramesElapsed() >= fadeStartTime)
		{
			int fadeLength = fadeEndTime - fadeStartTime;
			int alpha;
			if (m_IntroTitleTimer.FramesElapsed() < fadeEndTime) 
				alpha = int((double(m_IntroTitleTimer.FramesElapsed() - fadeStartTime) / double(fadeLength))*255);
			else 
				alpha = 255;

			GAME_ENGINE->SetColor(COLOR(0, 0, 0, alpha));
			GAME_ENGINE->FillRect(0, 0, Game::WIDTH, Game::HEIGHT);
		}

		return;
	}

	GAME_ENGINE->DrawBitmap(SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::MAIN_MENU_SCREEN_BACKGROUND));

	int lineHeight = 16;
	int left = 0, top = 0;
	const int textStartY = 117;

	switch (m_ScreenShowing)
	{
	case Screen::START:
	{
	} break;
	case Screen::SAVE_SELECTION:
	{
		left = 80;
		top = textStartY;
		SMWFont::PaintPhrase("MARIO A ... 2", left, top, true);
		top += lineHeight;
		SMWFont::PaintPhrase("MARIO B ...EMPTY", left, top, true);
		top += lineHeight;
		SMWFont::PaintPhrase("MARIO C ...EMPTY", left, top, true);
		top += lineHeight;
		SMWFont::PaintPhrase("ERASE DATA", left, top, true);

		left -= 16;
		top = textStartY + m_CursorIndex * lineHeight;
		PaintCursor(left, top);
	} break;
	case Screen::ERASE_DATA:
	{
		GAME_ENGINE->SetColor(COLOR(0, 0, 0, 120));
		GAME_ENGINE->FillRect(0, 0, Game::WIDTH, Game::HEIGHT);

		left = 55;
		top = textStartY;
		SMWFont::PaintPhrase("ERASE MARIO A ... 2", left, top, true);
		top += lineHeight;
		SMWFont::PaintPhrase("ERASE MARIO B ...EMPTY", left, top, true);
		top += lineHeight;
		SMWFont::PaintPhrase("ERASE MARIO C ...EMPTY", left, top, true);
		top += lineHeight;
		SMWFont::PaintPhrase("END", left, top, true);

		left -= 16;
		top = textStartY + m_CursorIndex * lineHeight;
		PaintCursor(left, top);
	} break;
	case Screen::PARTY_SIZE:
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

	// Main title
	left = 25;
	top = 38;
	RECT2 srcRect = RECT2(0, 0, 208, 66);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::MAIN_MENU_SCREEN), left, top, srcRect);

	// Copyright
	left = 55;
	top += 156;
	srcRect = RECT2(208, 58, 359, 66);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::MAIN_MENU_SCREEN), left, top, srcRect);

	PaintBorder();

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
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::MAIN_MENU_SCREEN), left, top, srcRect);
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
	
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::MAIN_MENU_SCREEN), left, top, srcRect);

	GAME_ENGINE->SetWorldMatrix(matPrevWorld);
}