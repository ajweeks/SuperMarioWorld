//-----------------------------------------------------------------
// Game Engine 
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 

//-----------------------------------------------------------------
// Include Files
//-----------------------------------------------------------------
#include "stdafx.h"
#include "GameWinMain.h"

//ReportLiveObjects:
//To use any of these GUID values, you must include Initguid.h before you include DXGIDebug.h in your code.
#include <Initguid.h>
#include <dxgidebug.h>

#include "Game.h"

//-----------------------------------------------------------------
// Defines
//-----------------------------------------------------------------

#define GAME_ENGINE (GameEngine::GetSingleton())

//-----------------------------------------------------------------
// Windows Functions
//-----------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char * szCmdLine, int iCmdShow)
{

#pragma region memcheck
	//notify user if heap is corrupt
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL,0);

	// Enable run-time memory leak check for debug builds.
	#if defined(DEBUG) | defined(_DEBUG)
		_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

		typedef HRESULT(__stdcall *fPtr)(const IID&, void**); 
		HMODULE hDll = LoadLibraryA("dxgidebug.dll"); 
		fPtr DXGIGetDebugInterface = (fPtr)GetProcAddress(hDll, "DXGIGetDebugInterface"); 

		IDXGIDebug* pDXGIDebug;
		DXGIGetDebugInterface(__uuidof(IDXGIDebug), (void**)&pDXGIDebug);
		//_CrtSetBreakAlloc(334);
	#endif
		
#pragma endregion

	///////////////////////////////////////////////////////
	if (GAME_ENGINE == nullptr) return FALSE; // create the game engine singleton and exit if it fails

	int returnValue = 0;

	GAME_ENGINE->SetGame(new Game());
	returnValue = GAME_ENGINE->Run(hInstance, iCmdShow); // run the game engine and return the result

	// Destroy the game engine
	GameEngine::Destroy();

#if defined(DEBUG) | defined(_DEBUG)
	// unresolved external  
	if(pDXGIDebug) pDXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL,DXGI_DEBUG_RLO_ALL); 
    pDXGIDebug->Release();
#endif

	return returnValue;

}
