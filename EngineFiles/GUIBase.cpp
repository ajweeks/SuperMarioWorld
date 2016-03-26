//-----------------------------------------------------------------
// Game Engine
// C++ Source - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------
#include "stdafx.h"    // for compiler
#include "../stdafx.h" // for intellisense
#include "GUIBase.h"

//------------------------------------------------------------------------------
// GUIBase class definitions. Derived classes visualise a gui control using D2D draw operations
//------------------------------------------------------------------------------
//GUIBase::GUIBase() 
//{
//	Initialize();
//}
//
//GUIBase::GUIBase(const String& text):
//	m_OriginalText(text)
//{
//	Initialize();
//}

GUIBase::GUIBase(int left, int top, int width, int height, const String& textRef) :
	m_OriginalText(textRef)
{
	Initialize();
	m_BoundingRect = RECT2(left, top, left + width, top + height);
}

//GUIBase::GUIBase(const string& text) :
//m_FontPtr(nullptr),
//m_bTriggered(false),
//m_bEnabled(true)
//{
//	m_Text = String(text.c_str());
//	Initialize();
//}

void GUIBase::Initialize()
{
	m_DefaultForeColor = COLOR(0, 0, 0);
	m_DefaultBackColor = COLOR(240, 240, 240);
	m_ForeColor = COLOR(0, 0, 0);
	m_BackColor = COLOR(240, 240, 240);
	m_ClientRect = m_BoundingRect = {};
	m_FontPtr = new Font(String("Consolas"), 14);
	m_FontPtr->SetAlignVCenter();
	m_FontPtr->SetAlignHCenter();
	GameEngine::GetSingleton()->RegisterGUI(this);
}

GUIBase::~GUIBase()
{
	GameEngine::GetSingleton()->UnRegisterGUI(this);
	delete m_FontPtr;
}

//---------------------------
// Methods - Member functions
//---------------------------

// Sets a new font for this GUI element. 
// Params: String type: the font typetype
//         int height: the font height in pixels
// Example: myPtr->SetFont("Consolas", 14);
void GUIBase::SetFont(const String& typeRef, int height)
{
	if (this == nullptr) MessageBoxA(NULL, "TextBox::SetFont() called from a pointer that is a nullptr\nThe MessageBox that will appear after you close this MessageBox is the default error message from visual studio.", "GameEngine says NO", MB_OK);

	delete m_FontPtr;
	m_FontPtr = new Font(typeRef, (float)height);
	m_FontPtr->SetAlignVCenter();
	m_FontPtr->SetAlignHCenter();
	LimitTextLengthToClientArea();
}

void GUIBase::SetText(const String& text)
{
	if (this == nullptr) MessageBoxA(NULL, "TextBox::SetFont() called from a pointer that is a nullptr\nThe MessageBox that will appear after you close this MessageBox is the default error message from visual studio.", "GameEngine says NO", MB_OK);

	m_OriginalText = text;
	LimitTextLengthToClientArea();
}

String GUIBase::GetText() const
{
	if (this == nullptr) MessageBoxA(NULL, "TextBox::GetText() called from a pointer that is a nullptr\nThe MessageBox that will appear after you close this MessageBox is the default error message from visual studio.", "GameEngine says NO", MB_OK);

	return m_OriginalText;
}

bool GUIBase::PointInRect(RECT2 r, DOUBLE2 pt) const
{
	if (pt.x<r.left || pt.x>r.right || pt.y<r.top || pt.y>r.bottom)return false;
	return true;
	//OR
	//if (pt.x>r.left && pt.x<r.right && pt.y>r.top && pt.y<r.bottom)return true;
	//return false;
}

void GUIBase::SetEnabled(bool bEnable)
{
	if (this == nullptr) MessageBoxA(NULL, "TextBox::SetEnabled() called from a pointer that is a nullptr\nThe MessageBox that will appear after you close this MessageBox is the default error message from visual studio.", "GameEngine says NO", MB_OK);

	m_bEnabled = bEnable;
}

void GUIBase::SetBackColor(COLOR backColor)
{
	if (this == nullptr) MessageBoxA(NULL, "TextBox::SetBackColor() called from a pointer that is a nullptr\nThe MessageBox that will appear after you close this MessageBox is the default error message from visual studio.", "GameEngine says NO", MB_OK);

	m_BackColor = backColor;
}

void GUIBase::SetForeColor(COLOR foreColor)
{
	if (this == nullptr) MessageBoxA(NULL, "TextBox::SetForeColor() called from a pointer that is a nullptr\nThe MessageBox that will appear after you close this MessageBox is the default error message from visual studio.", "GameEngine says NO", MB_OK);
	m_ForeColor = foreColor;
}

void GUIBase::SetDefaultBackColor()
{
	if (this == nullptr) MessageBoxA(NULL, "TextBox::SetForeColor() called from a pointer that is a nullptr\nThe MessageBox that will appear after you close this MessageBox is the default error message from visual studio.", "GameEngine says NO", MB_OK);
	m_BackColor = m_DefaultBackColor;
}

void GUIBase::SetDefaultForeColor()
{
	if (this == nullptr) MessageBoxA(NULL, "TextBox::SetForeColor() called from a pointer that is a nullptr\nThe MessageBox that will appear after you close this MessageBox is the default error message from visual studio.", "GameEngine says NO", MB_OK);
	m_ForeColor = m_DefaultForeColor;
}

void GUIBase::LimitTextLengthToClientArea()
{
	// check that text fits in text box: 1 line and no spaces causing caret to float ouside bounds

	//first copy original back to m_Text
	m_Text = m_OriginalText;
	// create a text layout object to retrieve info about the layout
	IDWriteTextLayout *textLayoutPtr = nullptr;
	DWRITE_TEXT_METRICS textMetrix = {};
	bool bRepeat = false;
	do
	{
		// Convert multibyte string to wide-character string
		size_t length = (size_t)m_Text.Length() + 1; // include room for null terminator
		wchar_t* wTextArr = new wchar_t[length];
		size_t returnvalue = 0;
		mbstowcs_s(&returnvalue, wTextArr, length, m_Text.C_str(), length);
		GameEngine::GetSingleton()->GetDWriteFactory()->CreateTextLayout(wTextArr, m_Text.Length(), m_FontPtr->GetTextFormat(),
			(FLOAT)(m_ClientRect.right - m_ClientRect.left),
			(FLOAT)(m_ClientRect.bottom - m_ClientRect.top),
			&textLayoutPtr);
		delete wTextArr;

		textLayoutPtr->GetMetrics(&textMetrix);
		textLayoutPtr->Release();
		bRepeat = false;
		if (textMetrix.lineCount > 1 ||
			textMetrix.widthIncludingTrailingWhitespace > (m_ClientRect.right - m_ClientRect.left) /*||
																								   ((textMetrix.widthIncludingTrailingWhitespace > 0) && (textMetrix.height > (m_ClientRect.bottom - m_ClientRect.top)))*/
																								   )
		{
			// OutputDebugStringA("Text too big for GUI\n");
			// remove last character
			if (m_Text.Length() > 0) m_Text = m_Text.SubStr(0, m_Text.Length() - 1);
			bRepeat = true;
		}
	} while (bRepeat);
}