#include "stdafx.h"

#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

Bitmap* SpriteSheetManager::levelOneBackground = nullptr;
Bitmap* SpriteSheetManager::levelOneForeground = nullptr;

Bitmap* SpriteSheetManager::hud = nullptr;

SpriteSheet* SpriteSheetManager::smallMario = nullptr;
SpriteSheet* SpriteSheetManager::superMario = nullptr;

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
	levelOneForeground = new Bitmap(String("Resources/levels/01/Level01.png"));
	levelOneBackground = new Bitmap(String("Resources/background.bmp"));

	smallMario = new SpriteSheet(String("Resources/mario.png"), 8, 3, 18, 32);
	smallMario->SetTransparencyColor(COLOR(80, 128, 255));

	superMario = new SpriteSheet(String("Resources/super_mario.png"), 8, 3, 18, 32);
	superMario->SetTransparencyColor(COLOR(80, 128, 255));

	generalTiles = new SpriteSheet(String("Resources/general_tiles.png"), 14, 24, 16, 16);

	hud = new Bitmap(String("Resources/hud.png"));
	//hud->SetTransparencyColor(COLOR(255,0,255));
}

void SpriteSheetManager::Unload()
{
	delete levelOneBackground;
	delete levelOneForeground;

	delete smallMario;
	delete superMario;

	delete generalTiles;

	delete hud;
}
