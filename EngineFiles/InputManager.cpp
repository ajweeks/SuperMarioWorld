//-----------------------------------------------------------------
// Game Engine
// C++ Source - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------
#include "stdafx.h"
#include "../stdafx.h"

#include "InputManager.h"

//------------------------------------------------------------------------------
// InputManager class definitions. Manages all input
//------------------------------------------------------------------------------

// Static initializaton
BYTE* InputManager::m_pCurrKeyboardState = nullptr;
BYTE* InputManager::m_pOldKeyboardState = nullptr;
BYTE* InputManager::m_pKeyboardState0 = nullptr;
BYTE* InputManager::m_pKeyboardState1 = nullptr;
bool  InputManager::m_KeyboardState0Active = true;
DOUBLE2 InputManager::m_OldMousePosition;
DOUBLE2 InputManager::m_CurrMousePosition;
DOUBLE2 InputManager::m_MouseMovement;
bool  InputManager::m_Enabled = true;

InputManager::InputManager(void)
{
}

InputManager::~InputManager(void)
{
	if (m_pKeyboardState0 != nullptr)
	{
		delete[] m_pKeyboardState0;
		delete[] m_pKeyboardState1;

		m_pKeyboardState0 = nullptr;
		m_pKeyboardState1 = nullptr;
		m_pCurrKeyboardState = nullptr;
		m_pOldKeyboardState = nullptr;
	}
}

void InputManager::Initialize()
{
	if (m_pKeyboardState0 == nullptr)
	{
		m_pKeyboardState0 = new BYTE[256];
		m_pKeyboardState1 = new BYTE[256];

		GetKeyboardState(m_pKeyboardState0);
		GetKeyboardState(m_pKeyboardState1);
		// prevent nullptr
		m_pCurrKeyboardState = m_pKeyboardState0;
		m_pOldKeyboardState = m_pKeyboardState1;
	}
}

bool InputManager::UpdateKeyboardStates()
{
	//Get Current KeyboardState and set Old KeyboardState
	BOOL getKeyboardResult;
	if (m_KeyboardState0Active)
	{
		// using windows messages
		getKeyboardResult = GetKeyboardState(m_pKeyboardState1);
		m_pOldKeyboardState = m_pKeyboardState0;
		m_pCurrKeyboardState = m_pKeyboardState1;
	}
	else
	{
		// using windows messages
		getKeyboardResult = GetKeyboardState(m_pKeyboardState0);
		m_pOldKeyboardState = m_pKeyboardState1;
		m_pCurrKeyboardState = m_pKeyboardState0;
	}

	m_KeyboardState0Active = !m_KeyboardState0Active;

	return true;// getKeyboardResult > 0 ? true : false;
}

void InputManager::Update()
{
	if (!m_Enabled)
		return;

	UpdateKeyboardStates();

	//Mouse Position
	m_OldMousePosition = m_CurrMousePosition;
	POINT mousePos;
	if (GetCursorPos(&mousePos))
	{
		ScreenToClient(GameEngine::GetSingleton()->GetWindow(), &mousePos);
		m_CurrMousePosition.Set(mousePos.x, mousePos.y);
	}

	m_MouseMovement.x = m_CurrMousePosition.x - m_OldMousePosition.x;
	m_MouseMovement.y = m_CurrMousePosition.y - m_OldMousePosition.y;
}

bool InputManager::IsKeyboardKeyDown(int key, bool previousFrame) const
{
	if (!m_pCurrKeyboardState || !m_pOldKeyboardState)
		return false;

	if (key > 0x07 && key <= 0xFE)
		return IsKeyboardKeyDown_unsafe(key, previousFrame);

	return false;
}

bool InputManager::IsMouseButtonDown(int button, bool previousFrame) const
{
	if (button > 0x00 && button <= 0x06)
		return IsMouseButtonDown_unsafe(button, previousFrame);

	return false;
}

bool InputManager::IsKeyboardKeyPressed(int key, bool previousFrame) const
{
	if (!m_pCurrKeyboardState || !m_pOldKeyboardState)
		return false;

	if (key > 0x07 && key <= 0xFE)
	{
		//previous frame not pressed, current frame pressed
		if (!IsKeyboardKeyDown_unsafe(key, true) && IsKeyboardKeyDown_unsafe(key))
		{
			return true;
		}
	}
	return false;
}

bool InputManager::IsMouseButtonPressed(int button, bool previousFrame) const
{
	if (button > 0x00 && button <= 0x06)
	{
		//previous frame not pressed, current frame pressed
		if (!IsMouseButtonDown_unsafe(button, true) && IsMouseButtonDown_unsafe(button))
		{
			return true;
		}
	}
	return false;
}

bool InputManager::IsKeyboardKeyReleased(int key, bool previousFrame) const
{
	if (!m_pCurrKeyboardState || !m_pOldKeyboardState)
		return false;

	if (key > 0x07 && key <= 0xFE)
	{
		//previous frame pressed, current frame not pressed
		if (IsKeyboardKeyDown_unsafe(key, true) && !IsKeyboardKeyDown_unsafe(key))
		{
			return true;
		}
	}
	return false;
}

bool InputManager::IsMouseButtonReleased(int button, bool previousFrame) const
{
	if (button > 0x00 && button <= 0x06)
	{
		//previous frame pressed, current frame not pressed
		if (IsMouseButtonDown_unsafe(button, true) && !IsMouseButtonDown_unsafe(button))
		{
			return true;
		}
	}
	return false;
}

//NO RANGE CHECKS
bool InputManager::IsKeyboardKeyDown_unsafe(int key, bool previousFrame) const
{
	if (previousFrame)
		return (m_pOldKeyboardState[key] & 0xF0) != 0;
	else
		return (m_pCurrKeyboardState[key] & 0xF0) != 0;
}

//NO RANGE CHECKS
bool InputManager::IsMouseButtonDown_unsafe(int button, bool previousFrame) const
{
	if (previousFrame)
		return (m_pOldKeyboardState[button] & 0xF0) != 0;
	else
		return (m_pCurrKeyboardState[button] & 0xF0) != 0;
}

//void InputManager::KeyboardKeyPressed(BYTE key)
//{
//	if (m_KeyboardState0Active)
//	{
//		m_pKeyboardState1[key] |= 0xF0;
//	}
//	else
//	{
//		m_pKeyboardState0[key] |= 0xF0;
//	}
//}
//
//void InputManager::KeyboardKeyReleased(BYTE key)
//{
//	if (m_KeyboardState0Active)
//	{
//		m_pKeyboardState1[key] = 0;
//	}
//	else
//	{
//		m_pKeyboardState0[key] |= 0;
//	}
//}

