#include "stdafx.h"

#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "Enumerations.h"

Bitmap* SpriteSheetManager::levelOneBackgroundPtr = nullptr;
Bitmap* SpriteSheetManager::levelOneUndergroundBackgroundPtr = nullptr;
std::vector<Bitmap*> SpriteSheetManager::levelForegroundPtrArr = std::vector<Bitmap*>(Constants::NUM_LEVELS);

Bitmap* SpriteSheetManager::hudPtr = nullptr;

Bitmap* SpriteSheetManager::mainMenuScreenPtr = nullptr;
Bitmap* SpriteSheetManager::mainMenuScreenBGPtr = nullptr;
Bitmap* SpriteSheetManager::fontPtr = nullptr;

SpriteSheet* SpriteSheetManager::smallMarioPtr = nullptr;
SpriteSheet* SpriteSheetManager::superMarioPtr = nullptr;
SpriteSheet* SpriteSheetManager::generalTilesPtr = nullptr;
SpriteSheet* SpriteSheetManager::beanstalkPtr = nullptr;

Bitmap* SpriteSheetManager::starParticlePtr = nullptr;
Bitmap* SpriteSheetManager::splatParticlePtr = nullptr;
Bitmap* SpriteSheetManager::oneUpParticlePtr = nullptr;
SpriteSheet* SpriteSheetManager::coinCollectParticlePtr = nullptr;
SpriteSheet* SpriteSheetManager::runningDustCloudParticlePtr = nullptr;
SpriteSheet* SpriteSheetManager::numberParticlePtr = nullptr;
SpriteSheet* SpriteSheetManager::enemyDeathCloudParticlePtr = nullptr;

SpriteSheet* SpriteSheetManager::montyMolePtr = nullptr;
SpriteSheet* SpriteSheetManager::koopaTroopaPtr = nullptr;
SpriteSheet* SpriteSheetManager::koopaShellPtr = nullptr;
SpriteSheet* SpriteSheetManager::piranhaPlantPtr = nullptr;
SpriteSheet* SpriteSheetManager::charginChuckPtr = nullptr;

SpriteSheet* SpriteSheetManager::yoshiPtr = nullptr;
SpriteSheet* SpriteSheetManager::smallYoshiPtr = nullptr;
SpriteSheet* SpriteSheetManager::yoshiWithMarioPtr = nullptr;



SpriteSheetManager::SpriteSheetManager()
{
}
SpriteSheetManager::~SpriteSheetManager()
{
}

void SpriteSheetManager::Load()
{
	// Level
	levelForegroundPtrArr[0] = new Bitmap(String("Resources/levels/00/foreground.png"));
	levelOneBackgroundPtr = new Bitmap(String("Resources/background.png"));
	
	levelForegroundPtrArr[1] = new Bitmap(String("Resources/levels/01/foreground.png"));
	levelOneUndergroundBackgroundPtr = new Bitmap(String("Resources/cave-bg-animated.png"));

	hudPtr = new Bitmap(String("Resources/hud.png"));

	mainMenuScreenPtr = new Bitmap(String("Resources/main-menu-screen.png"));
	mainMenuScreenBGPtr = new Bitmap(String("Resources/main-menu-bg.png"));
	fontPtr = new Bitmap(String("Resources/font.png"));

	// Mario
	smallMarioPtr = new SpriteSheet(String("Resources/mario.png"), 8, 3, 18, 32);
	smallMarioPtr->SetTransparencyColor(COLOR(80, 128, 255));

	superMarioPtr = new SpriteSheet(String("Resources/super-mario.png"), 8, 3, 18, 32);
	superMarioPtr->SetTransparencyColor(COLOR(80, 128, 255));
	generalTilesPtr = new SpriteSheet(String("Resources/general-tiles.png"), 14, 25, 16, 16);
	beanstalkPtr = new SpriteSheet(String("Resources/beanstalk.png"), 1, 3, 16, 16);

	// Particles
	starParticlePtr = new Bitmap(String("Resources/particles/star-particle.png"));
	splatParticlePtr = new Bitmap(String("Resources/particles/splat-particle.png"));
	oneUpParticlePtr = new Bitmap(String("Resources/particles/one-up-particle.png"));
	coinCollectParticlePtr = new SpriteSheet(String("Resources/particles/coin-collect-particle.png"), 10, 1, 16, 28);
	runningDustCloudParticlePtr = new SpriteSheet(String("Resources/particles/dust-cloud-particle.png"), 4, 1, 6, 6);
	numberParticlePtr = new SpriteSheet(String("Resources/particles/number-particle.png"), 10, 1, 4, 7);
	enemyDeathCloudParticlePtr = new SpriteSheet(String("Resources/particles/enemy-death-cloud-particle.png"), 5, 1, 22, 18);

	// Enemies
	montyMolePtr = new SpriteSheet(String("Resources/monty-mole.png"), 9, 1, 16, 16);
	koopaTroopaPtr = new SpriteSheet(String("Resources/koopa-troopa.png"), 8, 2, 16, 27);
	koopaShellPtr = new SpriteSheet(String("Resources/koopa-shell.png"), 2, 3, 16, 16);
	piranhaPlantPtr = new SpriteSheet(String("Resources/piranha-plant.png"), 4, 1, 16, 21);
	charginChuckPtr = new SpriteSheet(String("Resources/chargin-chuck.png"), 4, 3, 28, 27);

	yoshiPtr = new SpriteSheet(String("Resources/yoshi.png"), 5, 1, 26, 32);
	smallYoshiPtr = new SpriteSheet(String("Resources/yoshi-small.png"), 3, 2, 16, 16);
	yoshiWithMarioPtr = new SpriteSheet(String("Resources/yoshi-with-mario.png"), 13, 2, 33, 39);
}

void SpriteSheetManager::Unload()
{
	delete levelOneBackgroundPtr;
	delete levelOneUndergroundBackgroundPtr;

	for (size_t i = 0; i < levelForegroundPtrArr.size(); ++i)
	{
		delete levelForegroundPtrArr[i];
	}

	delete hudPtr;

	delete mainMenuScreenPtr;
	delete mainMenuScreenBGPtr;
	delete fontPtr;

	delete generalTilesPtr;

	delete smallMarioPtr;
	delete superMarioPtr;
	delete beanstalkPtr;

	delete yoshiPtr;
	delete smallYoshiPtr;
	delete yoshiWithMarioPtr;

	delete starParticlePtr;
	delete splatParticlePtr;
	delete oneUpParticlePtr;
	delete coinCollectParticlePtr;
	delete runningDustCloudParticlePtr;
	delete numberParticlePtr;
	delete enemyDeathCloudParticlePtr;

	delete koopaTroopaPtr;
	delete koopaShellPtr;
	delete montyMolePtr;
	delete piranhaPlantPtr;
	delete charginChuckPtr;
}
