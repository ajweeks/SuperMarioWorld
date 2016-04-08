#include "stdafx.h"

#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

Bitmap* SpriteSheetManager::levelOneBackgroundPtr = nullptr;
Bitmap* SpriteSheetManager::levelOneForegroundPtr = nullptr;

Bitmap* SpriteSheetManager::hudPtr = nullptr;

Bitmap* SpriteSheetManager::messageBox_01Ptr = nullptr;
Bitmap* SpriteSheetManager::messageBox_02Ptr = nullptr;

SpriteSheet* SpriteSheetManager::smallMarioPtr = nullptr;
SpriteSheet* SpriteSheetManager::superMarioPtr = nullptr;

SpriteSheet* SpriteSheetManager::generalTilesPtr = nullptr;

SpriteSheet* SpriteSheetManager::coinCollectParticlePtr = nullptr;
SpriteSheet* SpriteSheetManager::runningDustCloudParticlePtr = nullptr;
SpriteSheet* SpriteSheetManager::numberParticlePtr = nullptr;

SpriteSheetManager::SpriteSheetManager()
{
}
SpriteSheetManager::~SpriteSheetManager()
{
}

void SpriteSheetManager::Load()
{
	// TODO: Convert all sprite sheet files to the same file type?
	levelOneForegroundPtr = new Bitmap(String("Resources/levels/01/Level01.png"));
	levelOneBackgroundPtr = new Bitmap(String("Resources/background.bmp"));

	smallMarioPtr = new SpriteSheet(String("Resources/mario.png"), 8, 3, 18, 32);
	smallMarioPtr->SetTransparencyColor(COLOR(80, 128, 255));

	superMarioPtr = new SpriteSheet(String("Resources/super_mario.png"), 8, 3, 18, 32);
	superMarioPtr->SetTransparencyColor(COLOR(80, 128, 255));

	generalTilesPtr = new SpriteSheet(String("Resources/general_tiles.png"), 14, 24, 16, 16);

	hudPtr = new Bitmap(String("Resources/hud.png"));

	messageBox_01Ptr = new Bitmap(String("Resources/levels/01/message_box_01.png"));
	messageBox_02Ptr = new Bitmap(String("Resources/levels/01/message_box_02.png"));

	coinCollectParticlePtr = new SpriteSheet(String("Resources/coin-collect-particle.png"), 10, 1, 16, 28);
	runningDustCloudParticlePtr = new SpriteSheet(String("Resources/dust-cloud-particle.png"), 4, 1, 8, 12);
	numberParticlePtr = new SpriteSheet(String("Resources/number-particle.png"), 10, 1, 4, 7);

}

void SpriteSheetManager::Unload()
{
	delete levelOneBackgroundPtr;
	delete levelOneForegroundPtr;

	delete smallMarioPtr;
	delete superMarioPtr;

	delete generalTilesPtr;

	delete hudPtr;

	delete messageBox_01Ptr;
	delete messageBox_02Ptr;

	delete coinCollectParticlePtr;
	delete runningDustCloudParticlePtr;
	delete numberParticlePtr;

}
