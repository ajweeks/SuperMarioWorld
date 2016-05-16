#pragma once

#include "INT2.h"
#include "SoundManager.h"

struct LevelInfo
{
	const int m_Index;
	const String m_LevelSVGFilePath;
	const Bitmap* m_BmpBackgroundPtr;
	const int m_NumberOfBackgroundAnimationFrames; // Set this to -1 if the background doen't animate
	const Bitmap* m_BmpForegroundPtr;
	const SoundManager::SONG m_BackgroundMusic;
	const SoundManager::SONG m_BackgroundMusicFast;
	const int m_Width;
	const int m_Height;
};
