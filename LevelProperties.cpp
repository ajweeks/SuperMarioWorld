#include "stdafx.h"

#include "LevelProperties.h"
#include "Enumerations.h"
#include "SpriteSheetManager.h"
#include "Pipe.h"

std::vector<LevelProperties> LevelProperties::m_AllLevelPropertiesArr = std::vector<LevelProperties>(Constants::NUM_LEVELS);

void LevelProperties::Initialize()
{
	int index = 0;
	std::stringstream indexStream;

	indexStream << std::setw(2) << std::setfill('0') << index;
	m_AllLevelPropertiesArr[index] = {};
	m_AllLevelPropertiesArr[index].m_Index = index;
	m_AllLevelPropertiesArr[index].m_LevelSVGFilePath = String(("Resources/levels/" + indexStream.str() + "/level.svg").c_str());
	m_AllLevelPropertiesArr[index].m_BmpBackgroundPtr = SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::LEVEL_ONE_BACKGROUND);
	m_AllLevelPropertiesArr[index].m_NumberOfBackgroundAnimationFrames = -1;
	m_AllLevelPropertiesArr[index].m_BmpForegroundPtr = SpriteSheetManager::GetLevelForegroundBmpPtr(index);
	m_AllLevelPropertiesArr[index].m_BackgroundMusic = SoundManager::Song::OVERWORLD_BGM;
	m_AllLevelPropertiesArr[index].m_BackgroundMusicFast = SoundManager::Song::OVERWORLD_BGM_FAST;
	m_AllLevelPropertiesArr[index].m_Width = m_AllLevelPropertiesArr[index].m_BmpForegroundPtr->GetWidth();
	m_AllLevelPropertiesArr[index].m_Height = m_AllLevelPropertiesArr[index].m_BmpForegroundPtr->GetHeight();
	m_AllLevelPropertiesArr[index].m_TotalTime = 400;


	index = 1;
	indexStream.str("");
	indexStream << std::setw(2) << std::setfill('0') << index;
	m_AllLevelPropertiesArr[index] = {};
	m_AllLevelPropertiesArr[index].m_Index = index;
	m_AllLevelPropertiesArr[index].m_LevelSVGFilePath = String(("Resources/levels/" + indexStream.str() + "/level.svg").c_str());
	m_AllLevelPropertiesArr[index].m_BmpBackgroundPtr = SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::LEVEL_ONE_UNDERGROUND_BACKGROUND);
	m_AllLevelPropertiesArr[index].m_NumberOfBackgroundAnimationFrames = 3;
	m_AllLevelPropertiesArr[index].m_BmpForegroundPtr = SpriteSheetManager::GetLevelForegroundBmpPtr(index);
	m_AllLevelPropertiesArr[index].m_BackgroundMusic = SoundManager::Song::UNDERGROUND_BGM;
	m_AllLevelPropertiesArr[index].m_BackgroundMusicFast = SoundManager::Song::UNDERGROUND_BGM_FAST;
	m_AllLevelPropertiesArr[index].m_Width = m_AllLevelPropertiesArr[index].m_BmpForegroundPtr->GetWidth();
	m_AllLevelPropertiesArr[index].m_Height = m_AllLevelPropertiesArr[index].m_BmpForegroundPtr->GetHeight();
	m_AllLevelPropertiesArr[index].m_TotalTime = 400;
}

LevelProperties& LevelProperties::Get(int levelIndex)
{
	return m_AllLevelPropertiesArr[levelIndex];
}
