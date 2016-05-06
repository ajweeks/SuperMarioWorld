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
	static Bitmap* levelOneForegroundPtr;

	static Bitmap* hudPtr;

	static Bitmap* messageBox_01Ptr;
	static Bitmap* messageBox_02Ptr;
	
	static Bitmap* starParticlePtr;
	static Bitmap* splatParticlePtr;

	static Bitmap* mainMenuScreenPtr;
	static Bitmap* fontPtr;

	static SpriteSheet* smallMarioPtr;
	static SpriteSheet* superMarioPtr;
	static SpriteSheet* generalTilesPtr;
	static SpriteSheet* beanstalkPtr;

	static SpriteSheet* coinCollectParticlePtr;
	static SpriteSheet* runningDustCloudParticlePtr;
	static SpriteSheet* numberParticlePtr;
	static SpriteSheet* enemyDeathCloudParticlePtr;

	static SpriteSheet* montyMolePtr;
	static SpriteSheet* koopaTroopaPtr;
	static SpriteSheet* koopaShellPtr;

	static SpriteSheet* yoshiPtr;
	static SpriteSheet* smallYoshiPtr;
	static SpriteSheet* yoshiWithMarioPtr;

private:
	SpriteSheetManager();

};

