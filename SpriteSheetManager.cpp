#include "stdafx.h"

#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "Enumerations.h"

std::vector<Bitmap*> SpriteSheetManager::m_LevelForegroundsPtrArr = std::vector<Bitmap*>(Constants::NUM_LEVELS);

SpriteSheet* SpriteSheetManager::m_SpriteSheetPtrArr[];
Bitmap* SpriteSheetManager::m_BitmapPtrArr[];

SpriteSheetManager::SpriteSheetManager()
{
}
SpriteSheetManager::~SpriteSheetManager()
{
}

void SpriteSheetManager::Load()
{
	// Level
	m_LevelForegroundsPtrArr[0] = new Bitmap(String("Resources/levels/00/foreground.png"));
	m_LevelForegroundsPtrArr[1] = new Bitmap(String("Resources/levels/01/foreground.png"));

	m_BitmapPtrArr[int(Bitmaps::LEVEL_ONE_BACKGROUND)] = new Bitmap(String("Resources/background.png"));
	m_BitmapPtrArr[int(Bitmaps::LEVEL_ONE_UNDERGROUND_BACKGROUND)] = new Bitmap(String("Resources/cave-bg-animated.png"));

	m_BitmapPtrArr[int(Bitmaps::LEVEL_SELECT_BACKGROUND)] = new Bitmap(String("Resources/level-select-background.png"));
	m_BitmapPtrArr[int(Bitmaps::LEVEL_SELECT_WINDOW)] = new Bitmap(String("Resources/level-select-window.png"));
	m_SpriteSheetPtrArr[int(SpriteSheets::LEVEL_SELECT_MARIO)] = new SpriteSheet(String("Resources/level-select-mario.png"), 11, 2);

	m_BitmapPtrArr[int(Bitmaps::HUD)] = new Bitmap(String("Resources/hud.png"));
	m_SpriteSheetPtrArr[int(SpriteSheets::TITLE_TEXT)] = new SpriteSheet(String("Resources/title-text.png"), 1, 4);

	m_BitmapPtrArr[int(Bitmaps::MAIN_MENU_SCREEN)] = new Bitmap(String("Resources/main-menu-screen.png"));
	m_BitmapPtrArr[int(Bitmaps::MAIN_MENU_SCREEN_BACKGROUND)] = new Bitmap(String("Resources/main-menu-bg.png"));
	m_SpriteSheetPtrArr[int(SpriteSheets::FONT)] = new SpriteSheet(String("Resources/font.png"), 18, 8);
	m_SpriteSheetPtrArr[int(SpriteSheets::FONT_INVERTED)] = new SpriteSheet(String("Resources/font.png"), 18, 8);
	m_SpriteSheetPtrArr[int(SpriteSheets::FONT_INVERTED)]->GetBitmap()->Invert();

	// Mario
	m_SpriteSheetPtrArr[int(SpriteSheets::SMALL_MARIO)] = new SpriteSheet(String("Resources/small-mario.png"), 8, 3);
	m_SpriteSheetPtrArr[int(SpriteSheets::SUPER_MARIO)] = new SpriteSheet(String("Resources/super-mario.png"), 8, 3);
	
	m_SpriteSheetPtrArr[int(SpriteSheets::GENERAL_TILES)] = new SpriteSheet(String("Resources/general-tiles.png"), 6, 25);
	m_SpriteSheetPtrArr[int(SpriteSheets::BEANSTALK)] = new SpriteSheet(String("Resources/beanstalk.png"), 1, 3);

	// Particles
	m_BitmapPtrArr[int(Bitmaps::STAR_PARTICLE)] = new Bitmap(String("Resources/particles/star-particle.png"));
	m_BitmapPtrArr[int(Bitmaps::SPLAT_PARTICLE)] = new Bitmap(String("Resources/particles/splat-particle.png"));
	m_BitmapPtrArr[int(Bitmaps::ONE_UP_PARTICLE)] = new Bitmap(String("Resources/particles/one-up-particle.png"));
	m_BitmapPtrArr[int(Bitmaps::STAR_CLOUD_PARTICLE)] = new Bitmap(String("Resources/particles/star-cloud-particle.png"));
	m_SpriteSheetPtrArr[int(SpriteSheets::COIN_COLLECT_PARTICLE)] = new SpriteSheet(String("Resources/particles/coin-collect-particle.png"), 10, 1);
	m_SpriteSheetPtrArr[int(SpriteSheets::DUST_CLOUD_PARTICLE)] = new SpriteSheet(String("Resources/particles/dust-cloud-particle.png"), 4, 1);
	m_SpriteSheetPtrArr[int(SpriteSheets::NUMBER_PARTICLE)] = new SpriteSheet(String("Resources/particles/number-particle.png"), 10, 1);
	m_SpriteSheetPtrArr[int(SpriteSheets::ENEMY_DEATH_CLOUD_PARTICLE)] = new SpriteSheet(String("Resources/particles/enemy-death-cloud-particle.png"), 5, 1);
	m_SpriteSheetPtrArr[int(SpriteSheets::YOSHI_EGG_BREAK_PARTICLE)] = new SpriteSheet(String("Resources/particles/yoshi-egg-break-particle.png"), 4, 2);

	// Enemies
	m_SpriteSheetPtrArr[int(SpriteSheets::MONTY_MOLE)] = new SpriteSheet(String("Resources/monty-mole.png"), 9, 1);
	m_SpriteSheetPtrArr[int(SpriteSheets::KOOPA_TROOPA)] = new SpriteSheet(String("Resources/koopa-troopa.png"), 8, 2);
	m_SpriteSheetPtrArr[int(SpriteSheets::KOOPA_SHELL)] = new SpriteSheet(String("Resources/koopa-shell.png"), 2, 3);
	m_SpriteSheetPtrArr[int(SpriteSheets::PIRANHA_PLANT)] = new SpriteSheet(String("Resources/piranha-plant.png"), 4, 1);
	m_SpriteSheetPtrArr[int(SpriteSheets::CHARGIN_CHUCK)] = new SpriteSheet(String("Resources/chargin-chuck.png"), 5, 3);

	// Yoshi
	m_SpriteSheetPtrArr[int(SpriteSheets::YOSHI)] = new SpriteSheet(String("Resources/yoshi.png"), 4, 1);
	m_SpriteSheetPtrArr[int(SpriteSheets::SMALL_YOSHI)] = new SpriteSheet(String("Resources/yoshi-small.png"), 4, 2);
	m_SpriteSheetPtrArr[int(SpriteSheets::YOSHI_WITH_MARIO)] = new SpriteSheet(String("Resources/yoshi-with-mario.png"), 14, 2);
}

void SpriteSheetManager::Unload()
{
	for (size_t i = 0; i < m_LevelForegroundsPtrArr.size(); ++i)
	{
		delete m_LevelForegroundsPtrArr[i];
	}

	for (size_t i = 0; i < int(SpriteSheets::__LAST_ELEMENT); ++i)
	{
		delete m_SpriteSheetPtrArr[i];
	}
	for (size_t i = 0; i < int(Bitmaps::_LAST_ELEMENT); ++i)
	{
		delete m_BitmapPtrArr[i];
	}
}

Bitmap* SpriteSheetManager::GetLevelForegroundBmpPtr(int levelIndex)
{
	return m_LevelForegroundsPtrArr[levelIndex];
}

Bitmap* SpriteSheetManager::GetBitmapPtr(Bitmaps bitmap)
{
	return m_BitmapPtrArr[int(bitmap)];
}

SpriteSheet* SpriteSheetManager::GetSpriteSheetPtr(SpriteSheets spriteSheet)
{
	return m_SpriteSheetPtrArr[int(spriteSheet)];
}
