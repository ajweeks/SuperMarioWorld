#pragma once

#include "BaseState.h"

class Level;
struct SMWTimer;

class MainMenuState : public BaseState
{
public:
	// NOTE: This state will always be showing one of the following screens
	enum class Screen
	{
		START, SAVE_SELECTION, ERASE_DATA, PARTY_SIZE
	};

	MainMenuState(StateManager* stateManagerPtr);
	virtual ~MainMenuState();

	MainMenuState(const MainMenuState&) = delete;
	MainMenuState& operator=(const MainMenuState&) = delete;

	void Tick(double deltaTime);
	void Paint();

private:
	static const int SHORT_PLANK = 0;
	static const int LONG_PLANK = 1;
	static const int MED_PLANK = 2;

	void PaintBorder();
	void DrawBorderPlank(int plankIndex, int left, int top, bool flipV = false, bool flipH = false);
	void PaintCursor(int left, int top);
	void EnterScreen(Screen newScreen);

	static const int MAX_CURSOR_TIMER_VALUE = 35;
	int m_CursorAnimationTimer = 0;

	Screen m_ScreenShowing;
	int m_CursorIndex;
	int m_MaxCursorIndex;

	SMWTimer m_IntroTitleTimer; // Nintendo presents...
	SMWTimer m_IntroFadeInTimer; // Fade in from black
	SMWTimer m_OutroFadeOutTimer; // Fade out to black

};
