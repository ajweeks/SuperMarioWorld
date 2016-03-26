#include "stdafx.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

Bitmap* SpriteSheetManager::levelOneBackground = nullptr;
Bitmap* SpriteSheetManager::levelOneForeground = nullptr;

SpriteSheet* SpriteSheetManager::smallMario = nullptr;
SpriteSheet* SpriteSheetManager::generalTiles = nullptr;

SpriteSheetManager::SpriteSheetManager()
{
}
SpriteSheetManager::~SpriteSheetManager()
{
}

void SpriteSheetManager::Load()
{
	// TODO: Convert all sprite sheet files to the same file type?
	levelOneForeground = new Bitmap(String("Resources/levels/01/Level01x2.png"));
	levelOneBackground = new Bitmap(String("Resources/background.bmp"));

	smallMario = new SpriteSheet(String("Resources/mariox2.png"), 8, 3, 36, 64);
	smallMario->SetTransparencyColor(COLOR(80, 128, 255));

	generalTiles = new SpriteSheet(String("Resources/general_tilesx2.png"), 14, 24, 32, 32);
}

void SpriteSheetManager::Unload()
{
	delete levelOneBackground;
	delete levelOneForeground;

	delete smallMario;
	delete generalTiles;
}
