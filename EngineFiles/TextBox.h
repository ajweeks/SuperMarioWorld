//-----------------------------------------------------------------
// Game Engine
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

#pragma once

#include "GUIBase.h"

//-----------------------------------------------------------------
// TextBox Class
//-----------------------------------------------------------------
class TextBox : public GUIBase
{
public:
	//! Constructor
	//! @param left: the left coordinate of the bounding rectangle
	//! @param top:  the top coordinate of the bounding rectangle
	//! @param width: the width of the bounding rectangle
	//! @param height: the height of the bounding rectangle
	//! @param stringRef is optional: the text that will be appear in the TextBox
	//! Example: m_TxtPtr = new TextBox(10, 10, 120, 25, String("Some Text"));
	TextBox(int left, int top, int width, int height, const String& textRef = String());

	// Destructor
	virtual ~TextBox();

	// C++11 make the class non-copyable
	TextBox(const TextBox&) = delete;
	TextBox& operator=(const TextBox&) = delete;

	//-------------------------------------------------
	// Methods - Member functions							
	//-------------------------------------------------
	//! returns true if the the enter key was pressed while the textbox was active
	bool IsEntered() const;

	//! Sets the position and dimensions of the TextBox
	void SetBounds(int left, int top, int width, int height);
protected:

private:
	// Internal use only
	virtual void ConsumeEvent();
	// Internal use only
	virtual void Paint();
	// Internal use only
	virtual void Tick(double deltaTime);
	// Internal use only
	void DrawCaret();
	// Internal use only
	virtual void HandleKeyInput(char character);

	//-------------------------------------------------
	// Datamembers								
	//-------------------------------------------------
	// Has focus
	bool m_bArmed = false;
	// Frequency at wich the caret blinks
	double m_BlinkFrequency = 2;
	// Used for blinking
	double m_AccumulatedTime = 0;
	// caret blinks: draw or not? 
	bool m_bCaretBlinkState = true;
	// Client area inset: how many pixels from the bounds to the characters
	const int m_ClientInset = 2;
};