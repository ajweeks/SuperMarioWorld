#include "stdafx.h"

#include "LevelInfo.h"
#include "Enumerations.h"
#include "SpriteSheetManager.h"
#include "Pipe.h"

std::vector<LevelInfo> LevelInfo::levelInfoArr = std::vector<LevelInfo>(Constants::NUM_LEVELS);

void LevelInfo::Initialize()
{
	int index = 0;
	std::stringstream indexStream;

	indexStream << std::setw(2) << std::setfill('0') << index;
	levelInfoArr[index] = {};
	levelInfoArr[index].m_Index = index;
	levelInfoArr[index].m_LevelSVGFilePath = String(("Resources/levels/" + indexStream.str() + "/level.svg").c_str());
	levelInfoArr[index].m_BmpBackgroundPtr = SpriteSheetManager::levelOneBackgroundPtr;
	levelInfoArr[index].m_NumberOfBackgroundAnimationFrames = -1;
	levelInfoArr[index].m_IsUnderground = false;
	levelInfoArr[index].m_BmpForegroundPtr = SpriteSheetManager::levelForegroundPtrArr[index];
	levelInfoArr[index].m_BackgroundMusic = SoundManager::SONG::OVERWORLD_BGM;
	levelInfoArr[index].m_BackgroundMusicFast = SoundManager::SONG::OVERWORLD_BGM_FAST;
	levelInfoArr[index].m_Width = SpriteSheetManager::levelForegroundPtrArr[index]->GetWidth();
	levelInfoArr[index].m_Height = SpriteSheetManager::levelForegroundPtrArr[index]->GetHeight();

	index = 1;
	indexStream.str("");
	indexStream << std::setw(2) << std::setfill('0') << index;
	levelInfoArr[index] = {};
	levelInfoArr[index].m_Index = index;
	levelInfoArr[index].m_LevelSVGFilePath = String(("Resources/levels/" + indexStream.str() + "/level.svg").c_str());
	levelInfoArr[index].m_BmpBackgroundPtr = SpriteSheetManager::levelOneUndergroundBackgroundPtr;
	levelInfoArr[index].m_NumberOfBackgroundAnimationFrames = 3;
	levelInfoArr[index].m_IsUnderground = true;
	levelInfoArr[index].m_BmpForegroundPtr = SpriteSheetManager::levelForegroundPtrArr[index];
	levelInfoArr[index].m_BackgroundMusic = SoundManager::SONG::UNDERGROUND_BGM;
	levelInfoArr[index].m_BackgroundMusicFast = SoundManager::SONG::UNDERGROUND_BGM_FAST;
	levelInfoArr[index].m_Width = SpriteSheetManager::levelForegroundPtrArr[index]->GetWidth();
	levelInfoArr[index].m_Height = SpriteSheetManager::levelForegroundPtrArr[index]->GetHeight();
}