//-----------------------------------------------------------------
// Game Engine
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

#pragma once

class Bitmap
{
public:
	// -------------------------
	// Constructors & Destructor
	// -------------------------
	//! Load an Bitmap using a filename
	Bitmap(const String& fileNameRef);

	//! Load an Bitmap using a resourceID
	Bitmap(int resourceID);

	//! Load a Bitmap from memory
	Bitmap(BYTE* pBlob, int blobSize);

	virtual ~Bitmap();

	// C++11 make the class non-copyable
	Bitmap(const Bitmap&) = delete;
	Bitmap& operator=(const Bitmap&) = delete;

	//-------------------------------------------------
	// Methods							
	//-------------------------------------------------
	// Internal use only. Do not use this method unless you really know what you are doing
	ID2D1Bitmap*	GetBitmapPtr() const;

	//! Returns the width of this image
	int	GetWidth() const;

	//! Returns the height of this image
	int	GetHeight() const;

	//! Returns the opacity of this image
	double GetOpacity() const;

	//! The opacity level of an image has a range from 0 to 1
	//! The GameEngine uses the opacity level to blend the image with the background
	void SetOpacity(double opacity);

	//! Scans all pixels and turns the pixels with the given color transparent.
	//! Be carefull!! this is a very expensive operation. Use it in the GameStart only.
	//! alpha is ignored
	void SetTransparencyColor(COLOR transparentColor);

private:
	//---------------------------
	// Private methods
	//---------------------------
	// Not intended to be used by students
	HRESULT LoadResourceBitmap(ID2D1RenderTarget* renderTargetPtr, IWICImagingFactory* wICFactoryPtr, unsigned int resourceNumber, const String& resourceTypeRef, IWICFormatConverter** formatConvertorPtrPtr);
	// Not intended to be used by students
	HRESULT LoadBitmapFromFile(ID2D1RenderTarget* renderTargetPtr, IWICImagingFactory* wICFactoryPtr, const String& uriRef, UINT destinationWidth, UINT destinationHeight, IWICFormatConverter** formatConvertorPtrPtr);
	// Not intended to be used by students
	HRESULT LoadResourceFromStream(ID2D1RenderTarget* renderTargetPtr, IWICImagingFactory* wICFactoryPtr, byte* pBlob, int blobSize, IWICFormatConverter** formatConvertorPtr);

	//-------------------------------------------------
	// Datamembers								
	//-------------------------------------------------
	ID2D1Bitmap*			m_BitmapPtr;
	IWICFormatConverter*	m_ConvertorPtr;
	double					m_Opacity;//range: between 0 and 1
	String					m_FileName;
	int						m_ResourceID;
};
