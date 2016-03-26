//-----------------------------------------------------------------
// Game Engine
// C++ Source - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------
#include "stdafx.h"
#include "../stdafx.h"
#include "Font.h"


//---------------------------
// Font methods
//---------------------------
Font::Font(IDWriteTextFormat *textFormatPtr) : m_TextFormatPtr(textFormatPtr)
{}

Font::Font(const String& fontName, double size)
{
	//tstring temp(fontName.C_str());
	//std::wstring str(temp.begin(), temp.end());
	LoadTextFormat(fontName.C_str(), (float)size);
}


Font::~Font(void)
{
	m_TextFormatPtr->Release();
}

void Font::LoadTextFormat(const char* fontName, float size)
{
	HRESULT hr;
	// Create a DirectWrite text format object.
	// Convert multibyte string to wide-character string
	size_t length = strlen(fontName) + 1; // include room for null terminator
	wchar_t* wTextArr = new wchar_t[length];
	size_t returnvalue = 0;
	mbstowcs_s(&returnvalue, wTextArr, length, fontName, length);

	hr = GameEngine::GetSingleton()->GetDWriteFactory()->CreateTextFormat(wTextArr, NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size, L"", &m_TextFormatPtr);
	delete wTextArr;

	if (SUCCEEDED(hr))
	{
		// align left and top the text horizontally and vertically.
		m_TextFormatPtr->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		m_TextFormatPtr->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
	}
	else
	{
		MessageBoxA(NULL, fontName, "FONT BUILDING ERROR", MB_ICONERROR);
		exit(-1);
	}
}

IDWriteTextFormat* Font::GetTextFormat() const
{
	return m_TextFormatPtr;
}

void Font::SetAlignHLeft()
{
	m_TextFormatPtr->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
}

void Font::SetAlignHCenter()
{
	m_TextFormatPtr->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
}

void Font::SetAlignHRight()
{
	m_TextFormatPtr->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
}

void Font::SetAlignVTop()
{
	m_TextFormatPtr->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
}

void Font::SetAlignVCenter()
{
	m_TextFormatPtr->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
}

void Font::SetAlignVBottom()
{
	m_TextFormatPtr->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
}