#pragma once

#include "INT2.h"
#include "SoundManager.h"

class Pipe;

// This class represents all static data necessary to create a level (for entities see LevelData)
struct LevelProperties
{
	int m_Index;
	String m_LevelSVGFilePath;
	Bitmap* m_BmpBackgroundPtr;
	// Leave this set to -1 if the background doen't animate
	int m_NumberOfBackgroundAnimationFrames = -1; 
	Bitmap* m_BmpForegroundPtr;
	SoundManager::Song m_BackgroundMusic;
	SoundManager::Song m_BackgroundMusicFast;
	int m_Width;
	int m_Height;
	int m_TotalTime;

	static void Initialize();
	static LevelProperties& Get(int levelIndex);

private:
	static std::vector<LevelProperties> m_AllLevelPropertiesArr;
};
