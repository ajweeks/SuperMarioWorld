//-----------------------------------------------------------------
// Game Engine
// C++ Source - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------
#include "stdafx.h"
#include "../stdafx.h"
#include "FmodSystem.h"


FmodSystem::FmodSystem()
{
	Initialize();
}


FmodSystem::~FmodSystem()
{
	if (m_FmodSystemPtr)
	{
		m_FmodSystemPtr->release();
		m_FmodSystemPtr = nullptr;
	}
}

bool FmodSystem::ErrorCheck(FMOD_RESULT res)
{
	if (res != FMOD_OK)
	{
		std::wstringstream strstr;
		strstr << L"FMOD error! \n[" << res << L"] " << FMOD_ErrorString(res) << std::endl;
		MessageBoxW(NULL, strstr.str().c_str(), L"SOUND", MB_ICONERROR);
		return false;
	}

	return true;
}

void FmodSystem::Initialize()
{
	FMOD_RESULT result;
	unsigned int version;
	int numdrivers;

	/*
	Create a System object and initialize.
	*/
	result = FMOD::System_Create(&m_FmodSystemPtr);
	ErrorCheck(result);
	result = m_FmodSystemPtr->getVersion(&version);
	ErrorCheck(result);

	if (version < FMOD_VERSION)
	{
		std::wstringstream strstr;
		strstr << L"FMOD Initialization Failed!\n\nYou are using an old version of FMOD " << version << L". This program requires " << FMOD_VERSION << std::endl;
		MessageBoxW(NULL, strstr.str().c_str(), L"SOUND", MB_ICONERROR);
		return;
	}

	result = m_FmodSystemPtr->getNumDrivers(&numdrivers);
	ErrorCheck(result);
	if (numdrivers == 0)
	{
		result = m_FmodSystemPtr->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		ErrorCheck(result);
	}
	else
	{
		result = m_FmodSystemPtr->init(32, FMOD_INIT_NORMAL, 0);
		ErrorCheck(result);
	}
}
