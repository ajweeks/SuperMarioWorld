//-----------------------------------------------------------------
// Game Engine
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

#pragma once

#include "GUIBase.h"

class Bitmap;
//-----------------------------------------------------------------
// Button Class
//-----------------------------------------------------------------
class Button : public GUIBase
{
public:

	//! Constructor
	//! @param left: the left coordinate of the bounding rectangle
	//! @param top:  the top coordinate of the bounding rectangle
	//! @param width: the width of the bounding rectangle
	//! @param height: the height of the bounding rectangle
	//! @param stringRef: the text that will be appear in the button
	//! Example: m_BtnPtr = new Button(10, 10, 120, 25, String("Some Text"));
	Button(int left, int top, int width, int height, const String& textRef );

	//! Constructor
	//! @param left: the left coordinate of the bounding rectangle
	//! @param top:  the top coordinate of the bounding rectangle
	//! @param releasedFileNameRef: the name of the image displayed when released
	//! @param pressedFileNameRef: the name of the image displayed when pressed
	//! Example: m_BtnPtr = new Button(10, 10, String("Released image file"), String("Pressed image file"));
	Button(int left, int top, const String& releasedFileNameRef, const String& pressedFileNameRef);

	//! Constructor
	//! @param left: the left coordinate of the bounding rectangle
	//! @param top:  the top coordinate of the bounding rectangle
	//! @param releasedResourseID: the resourceID of the image displayed when released
	//! @param pressedResourseID: the resourceID of the image displayed when pressed
	//! Example: m_BtnPtr = new Button(10, 10, IDB_RELEASED, IDB_PRESSED);
	Button(int left, int top, int releasedResourseID, int pressedResourseID);

	//! Constructor
	//! @param left: the left coordinate of the bounding rectangle
	//! @param top:  the top coordinate of the bounding rectangle
	//! Example: m_BtnPtr = new Button(10, 10, String("Released image file"), String("Pressed image file"));
	//! @param releasedBlobPtr: the pointer to a blob of memory containing pixels to be displayed when released
	//! @param releasedBlobSize: the size in bytes of the released memory blob
	//! @param pressedBlobPtr: the pointer to a blob of memory containing pixels to be displayed when pressed
	//! @param pressedBlobSize: the size in bytes of the pressed memory blob
	Button(int left, int top, BYTE* releasedBlobPtr, int releasedBlobSize, BYTE* pressedBlobPtr, int pressedBlobSize);

	// Destructor
	virtual ~Button();		

	// C++11 make the class non-copyable
	Button(const Button&) = delete;
	Button& operator=(const Button&) = delete;

	//-------------------------------------------------
	// Methods - Member functions							
	//-------------------------------------------------

	//! Sets the position and dimensions of the Button
	void SetBounds(int left, int top, int width, int height);

	//! returns true if the button is pressed
	bool IsPressed() const;


private:
	//! Loads the bitmaps when used as imagebutton
	//! @param releasedFileNameRef: the name of the image displayed when released
	//! @param pressedFileNameRef: the name of the image displayed when pressed
	void LoadBitmaps(const String& releasedFileNameRef, const String& pressedFileNameRef);

	//! Loads the bitmaps when used as imagebutton
	//! @param releasedResourseID: the resourceID of the image displayed when released
	//! @param pressedResourseID: the resourceID of the image displayed when pressed
	void LoadBitmaps(int releasedResourseID, int pressedResourseID);

	//! Loads the bitmaps when used as imagebutton
	//! @param releasedBlobPtr: the pointer to a blob of memory containing pixels to be displayed when released
	//! @param releasedBlobSize: the size in bytes of the released memory blob
	//! @param pressedBlobPtr: the pointer to a blob of memory containing pixels to be displayed when pressed
	//! @param pressedBlobSize: the size in bytes of the pressed memory blob
	void LoadBitmaps(BYTE* releasedBlobPtr, int releasedBlobSize, BYTE* pressedBlobPtr, int pressedBlobSize);

	// Internal use only
	// if true, the bitmaps will be used to visualize the button
	void AutoSetImageMode();
	virtual void ConsumeEvent();
	virtual void Paint();
	virtual void Tick(double deltaTime);
	void DrawClassicButton();
	void DrawImageButton();

	//-------------------------------------------------
	// Datamembers								
	//-------------------------------------------------
	bool m_bArmed = false, m_bImageMode = false;
	Bitmap* m_BmpReleasedPtr = nullptr, *m_BmpPressedPtr = nullptr;
};