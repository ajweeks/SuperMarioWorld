//-----------------------------------------------------------------
// Game Engine
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------
#pragma once
#include "FMOD/inc/fmod.hpp"
#include "FMOD/inc/fmod_errors.h"


class FmodSound
{
public:
	FmodSound(void);//private constructor to prevent the user to use the new operator to create an instance
	virtual ~FmodSound(void);

	//! Loads a sound into memory
	void CreateSound(const String &filenameRef);

	//! Loads a sound into memory
	void CreateSound(int resourceID);

	//! Loads a sound from memory
	void CreateSound(BYTE* blobPtr, int blobSize);

	//! Opens a sound for streaming.
	void CreateStream(const String &filenameRef, bool loop = false);

	//! Opens a sound for streaming.
	void CreateStream(int resourceID, bool loop = false);

	//! Loads a sound from memory for streaming
	void CreateStream(BYTE* blobPtr, int blobSize, bool loop = false);

	//! Playback control. Plays the sound
	//! @param paused: True or false flag to specify whether to start the channel paused or not. Starting a channel paused allows the user to alter its attributes without it being audible, and unpausing with Channel::setPaused actually starts the sound. 
	//! @return: returns the FMOD channel, can be used to set the loop mode. Channel turns invalid if the sound stops.
	FMOD::Channel * Play(bool paused = false);

	//! Playback control. Pauzes the sound
	//! @param paused: if true, the sound is pauzed. \nIf false, the sound continues the playback.
	void SetPaused(bool paused);

	//! Playback control. Stops the sound from playing.
	void Stop();

	//! Returns true if the sound is playing, and false if not
	bool IsPlaying();

	//! Sets the sound to a point in time. Use 0 to "rewind" the sound.
	void SetPosition(unsigned int position);

	//! Returns true if the sound is paused
	//! Returns false if the sound is paused puased
	bool IsPaused();

	//! Sets a channel to loop a specified number of times before stopping.
	//! @param loopcount: 	Number of times to loop before stopping. 0 = oneshot, 1 = loop once then stop, -1 = loop forever, default = -1.
	void SetLoopCount(int loopcount);

	//! Retrieves the current loop count for the specified channel.
	//! @return loopcount: This function returns the loop count set by SetLoopCount.
	int GetLoopCount();

	//! Retrieves the current loop count for the specified channel.
	//! @return loopcount: This function retrieves the current loop countdown value 
	//! for the channel being played. This means it will decrement until reaching 0, as it plays. To reset the value, use Channel::setLoopCount.
	int GetCurrentLoopCount();

	//! Volume level can be below 0 to invert a signal and above 1 to amplify the signal.
	//! Note that increasing the signal level too far may cause audible distortion.
	//! Range from 0.0 to 1.0
	void SetVolume(double volume = 1.0);

	//! Return the volume
	double GetVolume();

	//! Returns the FMOD channel: usefull for adding extra functionality
	FMOD::Channel * GetFmodChannel() { return m_ChannelPtr; }

	//! Returns the FMOD Sound object pointer: useful for adding extra functionality
	FMOD::Sound * GetFmodSound() { return m_SoundPtr; }


	// C++11 make the class non-copyable
	FmodSound(const FmodSound& t) = delete;
	FmodSound& operator=(const FmodSound& t) = delete;

private:

	void ErrCheck(FMOD_RESULT m_FMODResult);
	void ErrCheck(FMOD_RESULT m_FMODResult, char *filepathPtr);
	void Extract(int resourceID, const String& typeRef, String &resultingFilenameRef);
	void ReleaseSound();
	void SetLoopMode();
	FMOD::Sound *m_SoundPtr = nullptr;
	FMOD::Channel *m_ChannelPtr = nullptr;
	FMOD_RESULT m_FMODResult = FMOD_RESULT::FMOD_OK;

	int m_LoopCount = 0;
	BYTE* m_bufferPtr = nullptr;
	int m_BufferSize = 0;
	bool m_DeleteBuffer = false;
};

