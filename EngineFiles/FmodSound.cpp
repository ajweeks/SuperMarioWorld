//-----------------------------------------------------------------
// Game Engine
// C++ Source - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------
#include "stdafx.h"
#include "../stdafx.h"

#include "FmodSound.h"
#include "FmodSystem.h"



FmodSound::FmodSound(void)
{
}

FmodSound::~FmodSound(void)
{
	ReleaseSound();
}

void FmodSound::ReleaseSound()
{
	if(m_SoundPtr)
	{
		m_FMODResult = m_SoundPtr->release();
		m_SoundPtr = nullptr;
		ErrCheck(m_FMODResult);

		if (m_bufferPtr != nullptr)
		{
			delete [] m_bufferPtr;
			m_bufferPtr = nullptr;
		}
	}
}

void FmodSound::CreateSound(const String &filenameRef)
{
	ReleaseSound();
	std::string name(filenameRef.C_str());
	m_FMODResult = GameEngine::GetSingleton()->GetFmodSystem()->GetSystem()->createSound(name.c_str(), FMOD_DEFAULT, 0, &m_SoundPtr);
	ErrCheck(m_FMODResult, (char*)name.c_str());
}

void FmodSound::CreateSound(int resourceID)
{
	String sType("SOUND");
	String fileName = String(resourceID) + String(".") + sType;
	String resultingFilename;
	Extract(resourceID, sType, resultingFilename);
	CreateSound(resultingFilename);
}

//! Buffer is copied into FMOD Memory (Cleanup required)
void FmodSound::CreateSound(BYTE* blobPtr, int blobSize)
{
	ReleaseSound();
	FMOD_CREATESOUNDEXINFO exInfo;
	ZeroMemory(&exInfo, sizeof(FMOD_CREATESOUNDEXINFO));
	exInfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	exInfo.length = blobSize;

	m_FMODResult = GameEngine::GetSingleton()->GetFmodSystem()->GetSystem()->createSound((char*)blobPtr, FMOD_DEFAULT | FMOD_OPENMEMORY, &exInfo, &m_SoundPtr);
	ErrCheck(m_FMODResult);
}

void FmodSound::CreateStream(const String &filenameRef, bool loop)
{
	ReleaseSound();
	std::string name(filenameRef.C_str());
	m_FMODResult = GameEngine::GetSingleton()->GetFmodSystem()->GetSystem()->createStream(name.c_str(), loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF, 0, &m_SoundPtr);
	ErrCheck(m_FMODResult, (char*)name.c_str());
}

void FmodSound::CreateStream(int resourceID, bool loop)
{
	String sType("SOUND");
	String fileName = String(resourceID) + String(".") + sType;
	String resultingFilename;
	Extract(resourceID, sType, resultingFilename);
	CreateStream(resultingFilename);
}

//!Buffer is NOT copied into FMOD memory!
void FmodSound::CreateStream(BYTE* blobPtr, int blobSize, bool loop)
{
	ReleaseSound();
	FMOD_CREATESOUNDEXINFO exInfo;
	ZeroMemory(&exInfo, sizeof(FMOD_CREATESOUNDEXINFO));
	exInfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	exInfo.length = blobSize;

	m_FMODResult = GameEngine::GetSingleton()->GetFmodSystem()->GetSystem()->createStream((char*)blobPtr, loop ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF | FMOD_OPENMEMORY, &exInfo, &m_SoundPtr);
	ErrCheck(m_FMODResult);

	m_bufferPtr = blobPtr;
	m_BufferSize = blobSize;
}

FMOD::Channel * FmodSound::Play(bool paused)
{
	//channel turns invalid when sound stops
	if (m_SoundPtr)
	{
		//create channel by playing in pauzed mode
		m_FMODResult = GameEngine::GetSingleton()->GetFmodSystem()->GetSystem()->playSound(m_SoundPtr, 0, true, &m_ChannelPtr);
		ErrCheck(m_FMODResult);

		//use the channel to set the loop mode
		//SetLoopMode();

		//play if needed
		if (!paused)
		{
			m_FMODResult = GameEngine::GetSingleton()->GetFmodSystem()->GetSystem()->playSound(m_SoundPtr, 0, paused, &m_ChannelPtr);
			ErrCheck(m_FMODResult);
		}
	}
	return m_ChannelPtr;
}
void FmodSound::SetLoopMode()
{
	if (m_ChannelPtr)
	{
		if (m_LoopCount != 0)
		{
			m_FMODResult = m_ChannelPtr->setMode(FMOD_LOOP_NORMAL);
			ErrCheck(m_FMODResult);
		}
		else
		{
			m_FMODResult = m_ChannelPtr->setMode(FMOD_LOOP_OFF);
			ErrCheck(m_FMODResult);
		}
		m_FMODResult = m_ChannelPtr->setLoopCount(m_LoopCount);
		ErrCheck(m_FMODResult);
	}
}
void FmodSound::SetPaused(bool paused)
{
	if (m_ChannelPtr)
	{
		m_FMODResult = m_ChannelPtr->setPaused(paused);
		ErrCheck(m_FMODResult);
	}
}

void FmodSound::Stop()
{
	if (m_ChannelPtr)
	{
		m_FMODResult = m_ChannelPtr->stop();
		ErrCheck(m_FMODResult);
	}
	m_ChannelPtr = nullptr;
}

bool FmodSound::IsPlaying()
{
	bool bIsPlaying = false;
	if (m_ChannelPtr)
	{
		m_FMODResult = m_ChannelPtr->isPlaying(&bIsPlaying);
		ErrCheck(m_FMODResult);
	}
	return bIsPlaying;
}

void FmodSound::SetPosition(unsigned int position)
{
	m_FMODResult = m_ChannelPtr->setPosition(position, FMOD_TIMEUNIT_MS);
	ErrCheck(m_FMODResult);
}

bool FmodSound::IsPaused()
{
	bool isPaused = false;
	m_FMODResult = m_ChannelPtr->getPaused(&isPaused);
	ErrCheck(m_FMODResult);
	return isPaused;
}

void FmodSound::SetLoopCount(int loopcount)
{
	m_LoopCount = loopcount;
}

int FmodSound::GetLoopCount()
{
	return m_LoopCount;
}

int FmodSound::GetCurrentLoopCount()
{
	int loopcount = 0;
	if (m_ChannelPtr)
	{
		m_FMODResult = m_ChannelPtr->getLoopCount(&loopcount);
		ErrCheck(m_FMODResult);
	}
	return loopcount;
}

void FmodSound::ErrCheck(FMOD_RESULT m_FMODResult)
{
    if (m_FMODResult != FMOD_OK)
    {
		char charArr[255];
        sprintf_s(charArr,255,"FMOD error! (%d) %s\n", m_FMODResult, FMOD_ErrorString(m_FMODResult));
		MessageBoxA(0,charArr,"ERROR",MB_ICONERROR);
        PostQuitMessage(0);
    }
}

void FmodSound::ErrCheck(FMOD_RESULT m_FMODResult, char *filepathPtr)
{
    if (m_FMODResult != FMOD_OK)
    {
		char charArr[255];
		sprintf_s(charArr, 255, "FMOD loading error! (%d) %s\nFile %s could not be loaded.\n", m_FMODResult, FMOD_ErrorString(m_FMODResult), filepathPtr);
		MessageBoxA(0,charArr,"ERROR",MB_ICONERROR);
        PostQuitMessage(0);
    }
}

void FmodSound::Extract(int resourceID, const String& typeRef, String &resultingFilenameRef)
{
	HRSRC hrsrc = FindResourceA(NULL, MAKEINTRESOURCEA(resourceID), typeRef.C_str());
	HGLOBAL hLoaded = LoadResource(NULL, hrsrc);
	LPVOID lpLock = LockResource(hLoaded);
	DWORD dwSize = SizeofResource(NULL, hrsrc);

	String path("temp/");
	CreateDirectoryA(path.C_str(), NULL);

	resultingFilenameRef = path + String(resourceID) + String(".") + typeRef;
	HANDLE hFile = CreateFileA(resultingFilenameRef.C_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwByteWritten = 0;
	WriteFile(hFile, lpLock, dwSize, &dwByteWritten, NULL);
	CloseHandle(hFile);
	FreeResource(hLoaded);
}

void FmodSound::SetVolume(double volume)
{
	if (m_ChannelPtr)
	{
		if (volume >= 0 && volume <= 1)
		{
			m_ChannelPtr->setVolume((float)volume);
		}
	}
}

double FmodSound::GetVolume()
{
	float volume = 0.0;
	if (m_ChannelPtr)
	{
		m_ChannelPtr->getVolume(&volume);
	}

	return (double)volume;
}