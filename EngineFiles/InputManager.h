//-----------------------------------------------------------------
// Game Engine
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

#pragma once

class InputManager
{
public:
	// private constructor; only GameEngine can create an object of this class
	~InputManager(void);

	// C++11 make the class non-copyable
	InputManager(const InputManager&) = delete;
	InputManager& operator=(const InputManager&) = delete;

	// Not intended to be used by students
	void Initialize();
	// Not intended to be used by students
	void Update();
	// Not intended to be used by students
	static void SetEnabled(bool enabled) { m_Enabled = enabled; }
	// Not intended to be used by students
	DOUBLE2 GetMousePosition(bool previousFrame = false) const { return (previousFrame) ? m_OldMousePosition : m_CurrMousePosition; }
	// Not intended to be used by students
	DOUBLE2 GetMouseMovement() const { return m_MouseMovement; }
	// Not intended to be used by students
	void CursorVisible(bool visible) { ShowCursor(visible); }
	// Not intended to be used by students
	bool IsKeyboardKeyDown(int key, bool previousFrame = false) const;
	// Not intended to be used by students
	bool IsMouseButtonDown(int button, bool previousFrame = false) const;
	// Not intended to be used by students
	bool IsKeyboardKeyPressed(int key, bool previousFrame = false) const;
	// Not intended to be used by students
	bool IsMouseButtonPressed(int button, bool previousFrame = false) const;
	// Not intended to be used by students
	bool IsKeyboardKeyReleased(int key, bool previousFrame = false) const;
	// Not intended to be used by students
	bool IsMouseButtonReleased(int button, bool previousFrame = false) const;


private:
	// only the gameengine can create this object
	friend class GameEngine;
	InputManager();

	static BYTE *m_pCurrKeyboardState, *m_pOldKeyboardState, *m_pKeyboardState0, *m_pKeyboardState1;
	static bool m_KeyboardState0Active;
	static DOUBLE2 m_CurrMousePosition, m_OldMousePosition, m_MouseMovement;

	static bool m_Enabled;

	// Not intended to be used by students
	bool UpdateKeyboardStates();
	// Not intended to be used by students
	bool IsKeyboardKeyDown_unsafe(int key, bool previousFrame = false) const;
	// Not intended to be used by students
	bool IsMouseButtonDown_unsafe(int button, bool previousFrame = false) const;


	// Game Engine captures windows messages and sends them to this manager
	//void KeyboardKeyPressed(BYTE key);
	// Game Engine captures windows messages and sends them to this manager
	//void KeyboardKeyReleased(BYTE key);
};