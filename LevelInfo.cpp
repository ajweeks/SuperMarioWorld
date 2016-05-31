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
	levelInfoArr[index].m_BmpBackgroundPtr = SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::LEVEL_ONE_BACKGROUND);
	levelInfoArr[index].m_NumberOfBackgroundAnimationFrames = -1;
	levelInfoArr[index].m_IsUnderground = false;
	levelInfoArr[index].m_BmpForegroundPtr = SpriteSheetManager::GetLevelForegroundBmpPtr(index);
	levelInfoArr[index].m_BackgroundMusic = SoundManager::Song::OVERWORLD_BGM;
	levelInfoArr[index].m_BackgroundMusicFast = SoundManager::Song::OVERWORLD_BGM_FAST;
	levelInfoArr[index].m_Width = levelInfoArr[index].m_BmpForegroundPtr->GetWidth();
	levelInfoArr[index].m_Height = levelInfoArr[index].m_BmpForegroundPtr->GetHeight();

	index = 1;
	indexStream.str("");
	indexStream << std::setw(2) << std::setfill('0') << index;
	levelInfoArr[index] = {};
	levelInfoArr[index].m_Index = index;
	levelInfoArr[index].m_LevelSVGFilePath = String(("Resources/levels/" + indexStream.str() + "/level.svg").c_str());
	levelInfoArr[index].m_BmpBackgroundPtr = SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::LEVEL_ONE_UNDERGROUND_BACKGROUND);
	levelInfoArr[index].m_NumberOfBackgroundAnimationFrames = 3;
	levelInfoArr[index].m_IsUnderground = true;
	levelInfoArr[index].m_BmpForegroundPtr = SpriteSheetManager::GetLevelForegroundBmpPtr(index);
	levelInfoArr[index].m_BackgroundMusic = SoundManager::Song::UNDERGROUND_BGM;
	levelInfoArr[index].m_BackgroundMusicFast = SoundManager::Song::UNDERGROUND_BGM_FAST;
	levelInfoArr[index].m_Width = levelInfoArr[index].m_BmpForegroundPtr->GetWidth();
	levelInfoArr[index].m_Height = levelInfoArr[index].m_BmpForegroundPtr->GetHeight();
}