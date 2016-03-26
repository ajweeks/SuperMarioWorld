//-----------------------------------------------------------------
// Game Engine
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

#pragma once

class Font
{
public:
	// -------------------------
	// Constructors & Destructor
	// -------------------------

	//! Provide a String containing a type name, 
	//! and a number representing the desired size of the font
	//! Example m_MyFont = new Font(String("Consolas"), 96);
	Font(const String& fontNameRef, double size);


	virtual ~Font();

	// C++11 make the class non-copyable
	Font(const Font&) = delete;
	Font& operator=(const Font&) = delete;

	//-------------------------------------------------
	// Methods							
	//-------------------------------------------------
	// Not intended to be used by students
	IDWriteTextFormat*	GetTextFormat() const;

	//! Horizontal left align 
	void	SetAlignHLeft();

	//! Horizontal center align 
	void	SetAlignHCenter();

	//! Horizontal right align 
	void	SetAlignHRight();

	//! Vertical top align
	void	SetAlignVTop();

	//! Vertical center align
	void	SetAlignVCenter();

	//! Vertical bottom allign
	void	SetAlignVBottom();

	//---------------------------
	// Private methods
	//---------------------------
private:
	friend class GameEngine;
	// Not intended to be used by students
	explicit Font(IDWriteTextFormat *textFormatPtr);

	// Not intended to be used by students
	void LoadTextFormat(const char* fontName, float size);

	//-------------------------------------------------
	// Datamembers								
	//-------------------------------------------------
	IDWriteTextFormat* m_TextFormatPtr;

};