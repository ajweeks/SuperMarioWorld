#pragma once

#include "INT2.h"
#include "SoundManager.h"

class Pipe;

struct LevelInfo
{
	int m_Index;
	String m_LevelSVGFilePath;
	Bitmap* m_BmpBackgroundPtr;
	// Leave this set to -1 if the background doen't animate
	int m_NumberOfBackgroundAnimationFrames = -1; 
	bool m_IsUnderground;
	Bitmap* m_BmpForegroundPtr;
	SoundManager::SONG m_BackgroundMusic;
	SoundManager::SONG m_BackgroundMusicFast;
	int m_Width;
	int m_Height;

	static void Initialize();
	static std::vector<LevelInfo> levelInfoArr;
};
