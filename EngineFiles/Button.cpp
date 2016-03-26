//-----------------------------------------------------------------
// Game Engine
// C++ Source - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------
#include "stdafx.h"    // for compiler
#include "../stdafx.h" // for intellisense

#include "Button.h"

//-----------------------------------------------------------------
// Button methods
//-----------------------------------------------------------------

Button::Button(int left, int top, int width, int height, const String& textRef) : GUIBase(left, top, width, height, textRef)
{
	m_DefaultBackColor = m_BackColor = COLOR(227, 227, 227);
	m_ClientRect = m_BoundingRect;
	LimitTextLengthToClientArea();
}
Button::Button(int left, int top, const String& releasedFileNameRef, const String& pressedFileNameRef) : GUIBase(left, top, 0, 0, String())
{
	LoadBitmaps(releasedFileNameRef, pressedFileNameRef);
	//m_DefaultBackColor = m_BackColor = COLOR(227, 227, 227);
	//m_ClientRect = m_BoundingRect;
	//LimitTextLengthToClientArea();
}
Button::Button(int left, int top, int releasedResourseID, int pressedResourseID) : GUIBase(left, top, 0, 0, String())
{
	LoadBitmaps(releasedResourseID, pressedResourseID);
	//m_DefaultBackColor = m_BackColor = COLOR(227, 227, 227);
	//m_ClientRect = m_BoundingRect;
	//LimitTextLengthToClientArea();
}
Button::Button(int left, int top, BYTE* releasedBlobPtr, int releasedBlobSize, BYTE* pressedBlobPtr, int pressedBlobSize):GUIBase(left, top, 0, 0, String())
{
	LoadBitmaps(releasedBlobPtr, releasedBlobSize, pressedBlobPtr, pressedBlobSize);
}

Button::~Button()
{
	delete m_BmpPressedPtr;
	delete m_BmpReleasedPtr;
}

//---------------------------
// Methods - Member functions
//---------------------------
void Button::SetBounds(int left, int top, int width, int height)
{
	if (this == nullptr) MessageBoxA(NULL, "Button::SetBounds() called from a pointer that is a nullptr\nThe MessageBox that will appear after you close this MessageBox is the default error message from visual studio.", "GameEngine says NO", MB_OK);

	m_BoundingRect = RECT2(left, top, left + width, top + height);
	m_ClientRect = m_BoundingRect;
	LimitTextLengthToClientArea();
}

void Button::Paint()
{
	// 27 april 2015: prevent translation, scaling and rotation 
	GameEngine::GetSingleton()->SetWorldMatrix(MATRIX3X2::CreateIdentityMatrix());

	if (m_BoundingRect.bottom - m_BoundingRect.top <= 0 ||
		m_BoundingRect.right - m_BoundingRect.left <= 0)
	{
		MessageBoxA(NULL, "Impossible to draw the Button, it has no valid bounds!", "GameEngine says NO", MB_OK);
		exit(-1);
	}
	// store original font
	Font *originalFont = GameEngine::GetSingleton()->GetFont();

	if (!m_bImageMode) DrawClassicButton();
	else DrawImageButton();

	//restore font
	GameEngine::GetSingleton()->SetFont(originalFont);
}

void Button::DrawClassicButton()
{
	// Draw the borders
	RECT2 r = m_BoundingRect;
	GameEngine::GetSingleton()->SetColor(COLOR(101, 101, 101));
	GameEngine::GetSingleton()->FillRect(r.left, r.top, r.right, r.bottom);

	++r.left; ++r.top; --r.right; --r.bottom;
	if (!m_bArmed) GameEngine::GetSingleton()->SetColor(COLOR(254, 254, 254));
	else GameEngine::GetSingleton()->SetColor(COLOR(101, 101, 101));
	GameEngine::GetSingleton()->FillRect(r.left, r.top, r.right, r.bottom);

	// Fill interior
	++r.left; ++r.top; --r.right; --r.bottom;
	GameEngine::GetSingleton()->SetColor(m_BackColor);
	GameEngine::GetSingleton()->FillRect(r.left, r.top, r.right, r.bottom);

	// Set the Font
	GameEngine::GetSingleton()->SetFont(m_FontPtr);

	if (!m_bArmed)
	{

		GameEngine::GetSingleton()->SetColor(COLOR(101, 101, 101));
		GameEngine::GetSingleton()->DrawLine(m_BoundingRect.right - 1, m_BoundingRect.top + 1, m_BoundingRect.right - 1, m_BoundingRect.bottom - 1);
		GameEngine::GetSingleton()->DrawLine(m_BoundingRect.left + 1, m_BoundingRect.bottom - 1, m_BoundingRect.right - 1, m_BoundingRect.bottom - 1);

		GameEngine::GetSingleton()->SetColor(COLOR(160, 160, 160));
		GameEngine::GetSingleton()->DrawLine(m_BoundingRect.right - 2, m_BoundingRect.top + 2, m_BoundingRect.right - 2, m_BoundingRect.bottom - 2);
		GameEngine::GetSingleton()->DrawLine(m_BoundingRect.left + 2, m_BoundingRect.bottom - 2, m_BoundingRect.right - 2, m_BoundingRect.bottom - 2);

		// Draw fore color when this is enabled
		if (m_bEnabled)GameEngine::GetSingleton()->SetColor(m_ForeColor);

		// gray when disabled
		else GameEngine::GetSingleton()->SetColor(COLOR(187, 187, 187));

		GameEngine::GetSingleton()->DrawString(m_Text, m_BoundingRect);
	}
	else
	{
		GameEngine::GetSingleton()->SetColor(COLOR(101, 101, 101));
		GameEngine::GetSingleton()->DrawLine(m_BoundingRect.left + 2, m_BoundingRect.top + 2, m_BoundingRect.right - 2, m_BoundingRect.top + 2);
		GameEngine::GetSingleton()->DrawLine(m_BoundingRect.left + 2, m_BoundingRect.top + 2, m_BoundingRect.left + 2, m_BoundingRect.bottom - 2);

		GameEngine::GetSingleton()->SetColor(COLOR(160, 160, 160));
		GameEngine::GetSingleton()->DrawLine(m_BoundingRect.left + 2, m_BoundingRect.top + 3, m_BoundingRect.right - 3, m_BoundingRect.top + 3);
		GameEngine::GetSingleton()->DrawLine(m_BoundingRect.left + 3, m_BoundingRect.top + 3, m_BoundingRect.left + 3, m_BoundingRect.bottom - 3);

		//++r.left; ++r.top; --r.right; --r.bottom;
		//if (!m_bArmed) GameEngine::GetSingleton()->SetColor(COLOR(240, 240, 240));
		//else GameEngine::GetSingleton()->SetColor(COLOR(180, 180, 180));
		////GameEngine::GetSingleton()->SetColor(COLOR(240, 240, 240));
		//GameEngine::GetSingleton()->FillRect(r.left, r.top, r.right, r.bottom);
		GameEngine::GetSingleton()->SetColor(m_ForeColor);
		GameEngine::GetSingleton()->DrawString(m_Text, m_BoundingRect.left + 2, m_BoundingRect.top + 2, m_BoundingRect.right + 2, m_BoundingRect.bottom + 2);
	}
}

void Button::DrawImageButton()
{
	if (m_bArmed)
	{
		GameEngine::GetSingleton()->DrawBitmap(m_BmpPressedPtr, m_BoundingRect.left, m_BoundingRect.top);
	}
	else
	{
		GameEngine::GetSingleton()->DrawBitmap(m_BmpReleasedPtr, m_BoundingRect.left, m_BoundingRect.top);
	}
}

void Button::Tick(double deltaTime)
{
	if (!m_bEnabled)
	{
		m_bArmed = false;
		return;
	}

	MATRIX3X2 matInverse = (GameEngine::GetSingleton()->GetWorldMatrix() * GameEngine::GetSingleton()->GetViewMatrix()).Inverse();

	DOUBLE2 mouseScreenSpace(GameEngine::GetSingleton()->GetMousePosition().x, GameEngine::GetSingleton()->GetMousePosition().y);
	DOUBLE2 mouseViewSpace = matInverse.TransformPoint(mouseScreenSpace);

	//RMB in button rect armes the button and paint will draw the pressed button
	if (GameEngine::GetSingleton()->IsMouseButtonDown(VK_LBUTTON) && PointInRect(m_BoundingRect, mouseViewSpace))
	{
		m_bArmed = true;
	}
	else
	{
		//if mouse button is released while in rect, then pressed is true
		if (m_bArmed && !GameEngine::GetSingleton()->IsMouseButtonDown(VK_LBUTTON) && PointInRect(m_BoundingRect, mouseViewSpace))
		{
			m_bTriggered = true;
			m_bArmed = false;
		}
		//while armed the RMB is released or outside the rect, then armed is false
		else if (m_bArmed && (!GameEngine::GetSingleton()->IsMouseButtonDown(VK_LBUTTON) || !PointInRect(m_BoundingRect, mouseViewSpace)))
		{
			m_bArmed = false;
		}
	}
}

bool Button::IsPressed() const
{
	if (this == nullptr) MessageBoxA(NULL, "TextBox::IsPressed() called from a pointer that is a nullptr\nThe MessageBox that will appear after you close this MessageBox is the default error message from visual studio.", "GameEngine says NO", MB_OK);

	return m_bTriggered;
}

void Button::ConsumeEvent()
{
	m_bTriggered = false;
}
void Button::LoadBitmaps(const String& releasedFileNameRef, const String& pressedFileNameRef)
{
	if (m_BmpPressedPtr) delete m_BmpPressedPtr;
	m_BmpPressedPtr = new Bitmap(pressedFileNameRef);

	if (m_BmpReleasedPtr) delete m_BmpReleasedPtr;
	m_BmpReleasedPtr = new Bitmap(releasedFileNameRef);

	AutoSetImageMode();
}

void Button::LoadBitmaps(int releasedResourseID, int pressedResourseID)
{
	if (m_BmpPressedPtr) delete m_BmpPressedPtr;
	m_BmpPressedPtr = new Bitmap(pressedResourseID);

	if (m_BmpReleasedPtr) delete m_BmpReleasedPtr;
	m_BmpReleasedPtr = new Bitmap(releasedResourseID);

	AutoSetImageMode();
}

void Button::LoadBitmaps(BYTE* releasedBlobPtr, int releasedBlobSize, BYTE* pressedBlobPtr, int pressedBlobSize)
{
	if (m_BmpPressedPtr) delete m_BmpPressedPtr;
	m_BmpPressedPtr = new Bitmap(pressedBlobPtr, pressedBlobSize);

	if (m_BmpReleasedPtr) delete m_BmpReleasedPtr;
	m_BmpReleasedPtr = new Bitmap(releasedBlobPtr, releasedBlobSize);

	AutoSetImageMode();
}

void Button::AutoSetImageMode()
{
	if (m_BmpPressedPtr != nullptr && m_BmpReleasedPtr != nullptr)
	{
		m_bImageMode = true;
		int maxWidth = max(m_BmpPressedPtr->GetWidth(), m_BmpReleasedPtr->GetWidth());
		m_BoundingRect.right = m_BoundingRect.left + maxWidth;
		int maxHeight = max(m_BmpPressedPtr->GetHeight(), m_BmpReleasedPtr->GetHeight());
		m_BoundingRect.bottom = m_BoundingRect.top + maxHeight;
	}
	else m_bImageMode = false;
	
}



