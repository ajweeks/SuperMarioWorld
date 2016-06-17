#pragma once

class SpriteSheet;

class SpriteSheetManager
{
public:
	enum SpriteSheets
	{
		SMALL_MARIO, SUPER_MARIO, GENERAL_TILES, BEANSTALK, FONT, FONT_INVERTED,
		COIN_COLLECT_PARTICLE, DUST_CLOUD_PARTICLE, NUMBER_PARTICLE, ENEMY_DEATH_CLOUD_PARTICLE, 
		YOSHI_EGG_BREAK_PARTICLE,
		MONTY_MOLE, KOOPA_TROOPA, KOOPA_SHELL, PIRANHA_PLANT, CHARGIN_CHUCK,
		YOSHI, SMALL_YOSHI, YOSHI_WITH_MARIO, 
		LEVEL_SELECT_MARIO, TITLE_TEXT,

		// NOTE: All entries must be above this line
		__LAST_ELEMENT
	};
	enum Bitmaps
	{
		LEVEL_ONE_BACKGROUND, LEVEL_ONE_UNDERGROUND_BACKGROUND,
		HUD, MAIN_MENU_SCREEN, MAIN_MENU_SCREEN_BACKGROUND, 
		STAR_PARTICLE, SPLAT_PARTICLE, ONE_UP_PARTICLE, STAR_CLOUD_PARTICLE,
		LEVEL_SELECT_BACKGROUND, LEVEL_SELECT_WINDOW,

		// NOTE: All entries must be above this line
		_LAST_ELEMENT
	};

	virtual ~SpriteSheetManager();

	SpriteSheetManager(const SpriteSheetManager&) = delete;
	SpriteSheetManager& operator=(const SpriteSheetManager&) = delete;

	static void Load();
	static void Unload();

	static Bitmap* GetLevelForegroundBmpPtr(int levelIndex);
	static Bitmap* GetBitmapPtr(Bitmaps bitmap);
	static SpriteSheet* GetSpriteSheetPtr(SpriteSheets spriteSheet);

private:
	SpriteSheetManager();

	static std::vector<Bitmap*> m_LevelForegroundsPtrArr;
	static SpriteSheet* m_SpriteSheetPtrArr[int(SpriteSheets::__LAST_ELEMENT)];
	static Bitmap* m_BitmapPtrArr[int(Bitmaps::_LAST_ELEMENT)];
};
