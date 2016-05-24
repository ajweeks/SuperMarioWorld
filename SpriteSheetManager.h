#pragma once

class SpriteSheet;

class SpriteSheetManager
{
public:
	virtual ~SpriteSheetManager();

	SpriteSheetManager(const SpriteSheetManager&) = delete;
	SpriteSheetManager& operator=(const SpriteSheetManager&) = delete;

	static void Load();
	static void Unload();

	static Bitmap* levelOneBackgroundPtr;
	static Bitmap* levelOneUndergroundBackgroundPtr;
	static std::vector<Bitmap*> levelForegroundPtrArr;

	static Bitmap* hudPtr;

	static Bitmap* mainMenuScreenPtr;
	static Bitmap* mainMenuScreenBGPtr;
	static Bitmap* fontPtr;

	static SpriteSheet* smallMarioPtr;
	static SpriteSheet* superMarioPtr;
	static SpriteSheet* generalTilesPtr;
	static SpriteSheet* beanstalkPtr;

	static Bitmap* starParticlePtr;
	static Bitmap* splatParticlePtr;
	static Bitmap* oneUpParticlePtr;
	static SpriteSheet* coinCollectParticlePtr;
	static SpriteSheet* runningDustCloudParticlePtr;
	static SpriteSheet* numberParticlePtr;
	static SpriteSheet* enemyDeathCloudParticlePtr;

	static SpriteSheet* montyMolePtr;
	static SpriteSheet* koopaTroopaPtr;
	static SpriteSheet* koopaShellPtr;
	static SpriteSheet* piranhaPlantPtr;
	static SpriteSheet* charginChuckPtr;

	static SpriteSheet* yoshiPtr;
	static SpriteSheet* smallYoshiPtr;
	static SpriteSheet* yoshiWithMarioPtr;

private:
	SpriteSheetManager();

};

