//-----------------------------------------------------------------
// Game Engine
// C++ Source - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------
#include "stdafx.h"    // for compiler
#include "../stdafx.h" // for intellisense

#include "Bitmap.h"

//---------------------------
// Bitmap methods
//---------------------------
// Load an Bitmap using a filename
Bitmap::Bitmap(const String& fileName) : m_BitmapPtr(nullptr), m_ConvertorPtr(nullptr), m_Opacity(1.0), m_ResourceID(0)
{
	m_FileName = fileName;
	//IWICFormatConverter *convertorPtr=nullptr;
	ID2D1RenderTarget *renderTargetPtr = GameEngine::GetSingleton()->GetHwndRenderTarget();
	IWICImagingFactory *iWICFactoryPtr = GameEngine::GetSingleton()->GetWICImagingFactory();

	HRESULT hr = LoadBitmapFromFile(renderTargetPtr, iWICFactoryPtr, m_FileName, 0, 0, &m_ConvertorPtr);
	if (SUCCEEDED(hr))
	{
		// Create a Direct2D bitmap from the WIC bitmap.
		hr = renderTargetPtr->CreateBitmapFromWicBitmap(m_ConvertorPtr, &m_BitmapPtr);
	}

	if (FAILED(hr))
	{
		//show messagebox and leave the program
		GameEngine::GetSingleton()->MessageBox(String("IMAGE LOADING ERROR File ") + fileName);
		exit(-1);
	}
}

// Load an Bitmap using a resourceID
Bitmap::Bitmap(int resourceID) : m_BitmapPtr(nullptr), m_ConvertorPtr(nullptr), m_Opacity(1), m_ResourceID(resourceID)
{
	ID2D1RenderTarget* renderTargetPtr = GameEngine::GetSingleton()->GetHwndRenderTarget();
	IWICImagingFactory* iWICFactoryPtr = GameEngine::GetSingleton()->GetWICImagingFactory();

	HRESULT hr = LoadResourceBitmap(renderTargetPtr, iWICFactoryPtr, (unsigned int)resourceID, String("IMAGE"), &m_ConvertorPtr);
	if (SUCCEEDED(hr))
	{
		//Create a Direct2D bitmap from the WIC bitmap.
		hr = renderTargetPtr->CreateBitmapFromWicBitmap(m_ConvertorPtr, &m_BitmapPtr);
	}

	if (FAILED(hr))
	{
		//show messagebox 
		GameEngine::GetSingleton()->MessageBox(String("RESOURCE IMAGE LOADING ERROR File. ID: ") + String(resourceID));
		exit(-1);
	}
}

// Load a Bitmap from memory
Bitmap::Bitmap(BYTE* pBlob, int blobSize) :m_BitmapPtr(nullptr), m_ConvertorPtr(nullptr), m_Opacity(1.0), m_ResourceID(0)
{
	ID2D1RenderTarget* renderTargetPtr = GameEngine::GetSingleton()->GetHwndRenderTarget();
	IWICImagingFactory* iWICFactoryPtr = GameEngine::GetSingleton()->GetWICImagingFactory();

	HRESULT hr = LoadResourceFromStream(renderTargetPtr, iWICFactoryPtr, pBlob, blobSize, &m_ConvertorPtr);
	if (SUCCEEDED(hr))
	{
		//Create a Direct2D bitmap from the WIC bitmap.
		hr = renderTargetPtr->CreateBitmapFromWicBitmap(m_ConvertorPtr, &m_BitmapPtr);
	}

	if (FAILED(hr))
	{
		//show messagebox 
		GameEngine::GetSingleton()->MessageBox(String("Image Stream Load Failure"));
		exit(-1);
	}
}

Bitmap::~Bitmap()
{
	m_BitmapPtr->Release();
	m_ConvertorPtr->Release();
}

HRESULT Bitmap::LoadResourceBitmap(ID2D1RenderTarget* renderTargetPtr, IWICImagingFactory* wICFactoryPtr, unsigned int resourceNumber, const String& resourceTypeRef, IWICFormatConverter** formatConverterPtr)
{
	HRESULT hr = S_OK;
	IWICBitmapDecoder* decoderPtr = nullptr;
	IWICBitmapFrameDecode* sourcePtr = nullptr;
	IWICStream* streamPtr = nullptr;

	HRSRC imageResHandle = 0;
	HGLOBAL imageResDataHandle = 0;
	void* bitmapFilePtr = nullptr;
	DWORD imageFileSize = 0;

	// Locate the resource. http://msdn.microsoft.com/en-us/library/ms648042(VS.85).aspx
	imageResHandle = FindResourceA(GetModuleHandle(0), MAKEINTRESOURCEA(resourceNumber), resourceTypeRef.C_str());

	hr = imageResHandle ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		// Load the resource.
		imageResDataHandle = LoadResource(GetModuleHandle(0), imageResHandle);

		hr = imageResDataHandle ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Lock it to get a system memory pointer.
		bitmapFilePtr = LockResource(imageResDataHandle);

		hr = bitmapFilePtr ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Calculate the size.
		imageFileSize = SizeofResource(GetModuleHandle(0), imageResHandle);

		hr = imageFileSize ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Create a WIC stream to map onto the memory.
		hr = wICFactoryPtr->CreateStream(&streamPtr);
	}

	if (SUCCEEDED(hr))
	{
		// Initialize the stream with the memory pointer and size.
		hr = streamPtr->InitializeFromMemory(reinterpret_cast<BYTE*>(bitmapFilePtr), imageFileSize);
	}

	if (SUCCEEDED(hr))
	{
		// Create a decoder for the stream.
		hr = wICFactoryPtr->CreateDecoderFromStream(streamPtr, NULL, WICDecodeMetadataCacheOnLoad, &decoderPtr);
	}

	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = decoderPtr->GetFrame(0, &sourcePtr);
	}

	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = wICFactoryPtr->CreateFormatConverter(formatConverterPtr);
	}

	if (SUCCEEDED(hr))
	{
		hr = (*formatConverterPtr)->Initialize(sourcePtr, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
	}

	if (decoderPtr != nullptr) decoderPtr->Release();
	if (sourcePtr != nullptr)  sourcePtr->Release();
	if (streamPtr != nullptr)  streamPtr->Release();

	return hr;
}

HRESULT Bitmap::LoadBitmapFromFile(ID2D1RenderTarget *renderTargetPtr, IWICImagingFactory *wICFactoryPtr, const String& uriRef, UINT destinationWidth, UINT destinationHeight, IWICFormatConverter **FormatConverterPtr)
{
	IWICBitmapDecoder*		decoderPtr = nullptr;
	IWICBitmapFrameDecode*	sourcePtr = nullptr;
	IWICBitmapScaler*		scalerPtr = nullptr;
	//CreateDecoderFromFilename->first param HAS TO BE LPCWSTR
	std::string tUri(uriRef.C_str());
	std::wstring wUri(tUri.begin(), tUri.end());
	HRESULT hr = wICFactoryPtr->CreateDecoderFromFilename(wUri.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoderPtr);

	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = decoderPtr->GetFrame(0, &sourcePtr);
	}
	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = wICFactoryPtr->CreateFormatConverter(FormatConverterPtr);
	}

	if (SUCCEEDED(hr))
	{
		// If a new width or height was specified, create an
		// IWICBitmapScaler and use it to resize the image.
		if (destinationWidth != 0 || destinationHeight != 0)
		{
			UINT originalWidth, originalHeight;
			hr = sourcePtr->GetSize(&originalWidth, &originalHeight);
			if (SUCCEEDED(hr))
			{
				if (destinationWidth == 0)
				{
					FLOAT scalar = (FLOAT)destinationHeight / (FLOAT)originalHeight;
					destinationWidth = (UINT)(scalar * (FLOAT)originalWidth);
				}
				else if (destinationHeight == 0)
				{
					FLOAT scalar = (FLOAT)destinationWidth / (FLOAT)originalWidth;
					destinationHeight = (UINT)(scalar * (FLOAT)originalHeight);
				}

				hr = wICFactoryPtr->CreateBitmapScaler(&scalerPtr);
				if (SUCCEEDED(hr))
				{
					hr = scalerPtr->Initialize(sourcePtr, destinationWidth, destinationHeight, WICBitmapInterpolationModeCubic);
				}
				if (SUCCEEDED(hr))
				{
					hr = (*FormatConverterPtr)->Initialize(scalerPtr, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
				}
			}
		}
		else // Don't scale the image.
		{
			hr = (*FormatConverterPtr)->Initialize(sourcePtr, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
		}
	}

	if (decoderPtr != nullptr) decoderPtr->Release();
	if (sourcePtr != nullptr)  sourcePtr->Release();
	if (scalerPtr != nullptr)  scalerPtr->Release();

	return hr;
}

HRESULT Bitmap::LoadResourceFromStream(ID2D1RenderTarget* renderTargetPtr, IWICImagingFactory* wICFactoryPtr, BYTE* pBlob, int blobSize, IWICFormatConverter** formatConvertorPtr)
{
	HRESULT hr = S_OK;
	IWICBitmapDecoder* decoderPtr = nullptr;
	IWICBitmapFrameDecode* sourcePtr = nullptr;
	IWICStream* streamPtr = nullptr;

	HRSRC imageResHandle = 0;
	HGLOBAL imageResDataHandle = 0;
	void* bitmapFilePtr = nullptr;
	DWORD imageFileSize = 0;


	if (SUCCEEDED(hr))
	{
		// Create a WIC stream to map onto the memory.
		hr = wICFactoryPtr->CreateStream(&streamPtr);
	}

	if (SUCCEEDED(hr))
	{
		// Initialize the stream with the memory pointer and size.
		hr = streamPtr->InitializeFromMemory(reinterpret_cast<BYTE*>(pBlob), blobSize);
	}

	if (SUCCEEDED(hr))
	{
		// Create a decoder for the stream.
		hr = wICFactoryPtr->CreateDecoderFromStream(streamPtr, NULL, WICDecodeMetadataCacheOnLoad, &decoderPtr);
	}

	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = decoderPtr->GetFrame(0, &sourcePtr);
	}

	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = wICFactoryPtr->CreateFormatConverter(formatConvertorPtr);
	}

	if (SUCCEEDED(hr))
	{
		hr = (*formatConvertorPtr)->Initialize(sourcePtr, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
	}

	if (decoderPtr != nullptr) decoderPtr->Release();
	if (sourcePtr != nullptr)  sourcePtr->Release();
	if (streamPtr != nullptr)  streamPtr->Release();

	return hr;
}

ID2D1Bitmap* Bitmap::GetBitmapPtr() const
{
	return m_BitmapPtr;
}

int Bitmap::GetWidth() const
{
	if (this == nullptr) MessageBoxA(NULL, "Bitmap::GetWidth() called from a pointer that is a nullptr\nThe MessageBox that will appear after you close this MessageBox is the default error message from visual studio.", "GameEngine says NO", MB_OK);
	return m_BitmapPtr->GetPixelSize().width;
}

int	Bitmap::GetHeight() const
{
	if (this == nullptr) MessageBoxA(NULL, "Bitmap::GetHeight() called from a pointer that is a nullptr\nThe MessageBox that will appear after you close this MessageBox is the default error message from visual studio.", "GameEngine says NO", MB_OK);
	return m_BitmapPtr->GetPixelSize().height;
}

double Bitmap::GetOpacity() const
{
	if (this == nullptr) MessageBoxA(NULL, "Bitmap::GetOpacity() called from a pointer that is a nullptr\nThe MessageBox that will appear after you close this MessageBox is the default error message from visual studio.", "GameEngine says NO", MB_OK);
	return m_Opacity;
}

void Bitmap::SetOpacity(double opacity)
{
	if (this == nullptr) MessageBoxA(NULL, "Bitmap::SetOpacity() called from a pointer that is a nullptr\nThe MessageBox that will appear after you close this MessageBox is the default error message from visual studio.", "GameEngine says NO", MB_OK);
	m_Opacity = opacity;
}

void Bitmap::SetTransparencyColor(COLOR transparentColor)
{
	if (this == nullptr) MessageBoxA(NULL, "Bitmap::SetTransparencyColor() called from a pointer that is a nullptr\nThe MessageBox that will appear after you close this MessageBox is the default error message from visual studio.", "GameEngine says NO", MB_OK);

	COLORREF color = RGB(transparentColor.red, transparentColor.green, transparentColor.blue);
	UINT width = 0, height = 0;
	WICPixelFormatGUID pixelFormat;
	m_ConvertorPtr->GetPixelFormat(&pixelFormat);
	m_ConvertorPtr->GetSize(&width, &height);
	UINT bitmapStride = 4 * width;
	UINT size = width * height * 4;
	unsigned char* pixelsPtr = new unsigned char[size]; // create 32 bit buffer
	m_ConvertorPtr->CopyPixels(NULL, bitmapStride, size, pixelsPtr);

	for (unsigned int count = 0; count < width * height; ++count)
	{
		if (RGB(pixelsPtr[count * 4 + 2], pixelsPtr[count * 4 + 1], pixelsPtr[count * 4]) == color) // if the color of this pixel == transparency color
		{
			((int*)pixelsPtr)[count] = 0; // set all four values to zero, this assumes sizeof(int) == 4 on this system
			// setting values to zero means premultiplying the RGB values to an alpha of 0
		}
	}

	//assign modified pixels to bitmap
	IWICImagingFactory* iWICFactoryPtr = GameEngine::GetSingleton()->GetWICImagingFactory();
	IWICBitmap* iWICBitmapPtr = nullptr;
	HRESULT hr = iWICFactoryPtr->CreateBitmapFromMemory(width, height, GUID_WICPixelFormat32bppPBGRA, bitmapStride, size, pixelsPtr, &iWICBitmapPtr);
	delete[] pixelsPtr; //destroy buffer
	if (hr == S_OK)
	{
		ID2D1RenderTarget *renderTargetPtr = GameEngine::GetSingleton()->GetHwndRenderTarget();
		if (m_BitmapPtr != nullptr) m_BitmapPtr->Release();
		renderTargetPtr->CreateBitmapFromWicBitmap(iWICBitmapPtr, &m_BitmapPtr);
		iWICBitmapPtr->Release();
	}
}