#include "stdafx.h"

#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

Bitmap* SpriteSheetManager::levelOneBackgroundPtr = nullptr;
Bitmap* SpriteSheetManager::levelOneForegroundPtr = nullptr;

Bitmap* SpriteSheetManager::hudPtr = nullptr;

Bitmap* SpriteSheetManager::messageBox_01Ptr = nullptr;
Bitmap* SpriteSheetManager::messageBox_02Ptr = nullptr;

Bitmap* SpriteSheetManager::starParticlePtr = nullptr;
Bitmap* SpriteSheetManager::splatParticlePtr = nullptr;


SpriteSheet* SpriteSheetManager::smallMarioPtr = nullptr;
SpriteSheet* SpriteSheetManager::superMarioPtr = nullptr;

SpriteSheet* SpriteSheetManager::generalTilesPtr = nullptr;
SpriteSheet* SpriteSheetManager::beanstalkPtr = nullptr;

SpriteSheet* SpriteSheetManager::coinCollectParticlePtr = nullptr;
SpriteSheet* SpriteSheetManager::runningDustCloudParticlePtr = nullptr;
SpriteSheet* SpriteSheetManager::numberParticlePtr = nullptr;
SpriteSheet* SpriteSheetManager::enemyDeathCloudParticlePtr = nullptr;

SpriteSheet* SpriteSheetManager::montyMolePtr = nullptr;
SpriteSheet* SpriteSheetManager::koopaTroopaPtr = nullptr;
SpriteSheet* SpriteSheetManager::koopaShellPtr = nullptr;


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

	hudPtr = new Bitmap(String("Resources/hud.png"));

	messageBox_01Ptr = new Bitmap(String("Resources/levels/01/message_box_01.png"));
	messageBox_02Ptr = new Bitmap(String("Resources/levels/01/message_box_02.png"));

	starParticlePtr = new Bitmap(String("Resources/star-particle.png"));
	splatParticlePtr = new Bitmap(String("Resources/splat-particle.png"));


	smallMarioPtr = new SpriteSheet(String("Resources/mario.png"), 8, 3, 18, 32);
	smallMarioPtr->SetTransparencyColor(COLOR(80, 128, 255));

	superMarioPtr = new SpriteSheet(String("Resources/super_mario.png"), 8, 3, 18, 32);
	superMarioPtr->SetTransparencyColor(COLOR(80, 128, 255));

	generalTilesPtr = new SpriteSheet(String("Resources/general_tiles.png"), 14, 24, 16, 16);
	beanstalkPtr = new SpriteSheet(String("Resources/beanstalk.png"), 1, 3, 16, 16);

	coinCollectParticlePtr = new SpriteSheet(String("Resources/coin-collect-particle.png"), 10, 1, 16, 28);
	runningDustCloudParticlePtr = new SpriteSheet(String("Resources/dust-cloud-particle.png"), 4, 1, 8, 12);
	numberParticlePtr = new SpriteSheet(String("Resources/number-particle.png"), 10, 1, 4, 7);
	enemyDeathCloudParticlePtr = new SpriteSheet(String("Resources/enemy-death-cloud-particle.png"), 5, 1, 22, 18);

	montyMolePtr = new SpriteSheet(String("Resources/monty-mole.png"), 9, 1, 16, 16);
	koopaTroopaPtr = new SpriteSheet(String("Resources/koopa-troopa.png"), 8, 2, 16, 27);
	koopaShellPtr = new SpriteSheet(String("Resources/koopa-shell.png"), 2, 3, 16, 16);
}

void SpriteSheetManager::Unload()
{
	delete levelOneBackgroundPtr;
	delete levelOneForegroundPtr;

	delete hudPtr;

	delete messageBox_01Ptr;
	delete messageBox_02Ptr;

	delete starParticlePtr;
	delete splatParticlePtr;
	delete enemyDeathCloudParticlePtr;


	delete smallMarioPtr;
	delete superMarioPtr;

	delete generalTilesPtr;
	delete beanstalkPtr;
	delete koopaTroopaPtr;
	delete koopaShellPtr;

	delete coinCollectParticlePtr;
	delete runningDustCloudParticlePtr;
	delete numberParticlePtr;

	delete montyMolePtr;
}
