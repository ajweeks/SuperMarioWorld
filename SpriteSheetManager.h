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

	static Bitmap* messageBox_01;
	static Bitmap* messageBox_02;

	static SpriteSheet* smallMario;
	static SpriteSheet* superMario;
	static SpriteSheet* generalTiles;

private:
	SpriteSheetManager();

};

