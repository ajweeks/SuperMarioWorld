#pragma once

class SoundManager
{
public:
	virtual ~SoundManager();

	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;

	static void InitialzeSounds();
	static void UnloadSounds();

	static void PlaySound(FmodSound* sound);

	static FmodSound* playerJumpSndPtr;
	static FmodSound* playerSpinJumpSndPtr;

	static FmodSound* coinCollectSndPtr;
	static FmodSound* dragonCoinCollectSndPtr;

	static FmodSound* blockHitSndPtr;

	static FmodSound* superMushroomSpawnSndPtr;

private:
	SoundManager();

};

