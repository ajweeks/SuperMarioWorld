#include "stdafx.h"
#include "SoundManager.h"

FmodSound* SoundManager::playerJumpSndPtr = nullptr;
FmodSound* SoundManager::playerSpinJumpSndPtr = nullptr;

FmodSound* SoundManager::coinCollectSndPtr = nullptr;
FmodSound* SoundManager::dragonCoinCollectSndPtr = nullptr;

FmodSound* SoundManager::blockHitSndPtr = nullptr;

FmodSound* SoundManager::superMushroomSpawnSndPtr = nullptr;

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}

void SoundManager::InitialzeSounds()
{
	playerJumpSndPtr = new FmodSound();
	playerJumpSndPtr->CreateSound(String("Resources/sound/player-jump.wav"));

	playerSpinJumpSndPtr = new FmodSound();
	playerSpinJumpSndPtr->CreateSound(String("Resources/sound/player-spin-jump.wav"));

	coinCollectSndPtr = new FmodSound();
	coinCollectSndPtr->CreateSound(String("Resources/sound/coin-collect.wav"));

	dragonCoinCollectSndPtr = new FmodSound();
	dragonCoinCollectSndPtr->CreateSound(String("Resources/sound/dragon-coin-collect.wav"));

	blockHitSndPtr = new FmodSound();
	blockHitSndPtr->CreateSound(String("Resources/sound/block-hit.wav"));

	superMushroomSpawnSndPtr = new FmodSound();
	superMushroomSpawnSndPtr->CreateSound(String("Resources/sound/super-mushroom-spawn.wav"));

}

void SoundManager::UnloadSounds()
{
	// TODO: Find out if we should be stopping the sounds before we delete them
	delete playerJumpSndPtr;
	delete playerSpinJumpSndPtr;
	delete coinCollectSndPtr;
	delete dragonCoinCollectSndPtr;
	delete blockHitSndPtr;
	delete superMushroomSpawnSndPtr;
}

void SoundManager::PlaySound(FmodSound* sound)
{
	//sound->SetPosition(0);
	sound->Play();
}