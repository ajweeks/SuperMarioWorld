#pragma once

struct Keybindings
{
	static void ReadBindingsFromFile();

	static int D_PAD_LEFT;
	static int D_PAD_RIGHT;
	static int D_PAD_UP;
	static int D_PAD_DOWN;

	static int X_BUTTON;
	static int Y_BUTTON;
	static int A_BUTTON;
	static int B_BUTTON;

	static int START_BUTTON;
	static int SELECT_BUTTON;
	
	static int LEFT_SHOULDER;
	static int RIGHT_SHOULDER;

	static int TOGGLE_INFO_OVERLAY;
	static int SHOW_NEXT_INFO_SESSION;
	static int SHOW_PREVIOUS_INFO_SESSION;
	static int SCROLL_THROUGH_SESSIONS;
	static int TOGGLE_MUTED;
	static int GENERATE_BAR_GRAPH;

	static int DEBUG_TELEPORT_PLAYER;
	static int DEBUG_QUICK_RESET;
	static int DEBUG_TOGGLE_PHYSICS_RENDERING;
	static int DEBUG_FRAME_BY_FRAME_ADVANCE;

private:
	Keybindings() = delete;

	static void StoreStringData(std::string fileContents);
	static int RegisterKeycode(std::string fileContents, std::string tagName, int defaultValue);
};
