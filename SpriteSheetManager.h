#pragma once

class SpriteSheet;

class SpriteSheetManager
{
public:
	virtual ~SpriteSheetManager();

	SpriteSheetManager(const SpriteSheetManager&) = delete;
	SpriteSheetManager&operator=(const SpriteSheetManager&) = delete;

	static void Load();
	static void Unload();

	static Bitmap* levelOneBackground;
	static Bitmap* levelOneForeground;

	static Bitmap* hud;

	static SpriteSheet* smallMario;
	static SpriteSheet* superMario;
	static SpriteSheet* generalTiles;

private:
	SpriteSheetManager();

};

