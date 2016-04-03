#include "stdafx.h"
#include "SoundManager.h"

FmodSound* SoundManager::sounds[];
FmodSound* SoundManager::songs[];

bool SoundManager::muted = false;
double SoundManager::globalVolumeLevel = 1.0;

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}

void SoundManager::InitialzeSounds()
{
	songs[int(SONG::OVERWORLD_BGM)] = new FmodSound();
	songs[int(SONG::OVERWORLD_BGM)]->CreateStream(String("Resources/sound/music/overworld-bgm.wav"), true);

	songs[int(SONG::OVERWORLD_BGM_FAST)] = new FmodSound();
	songs[int(SONG::OVERWORLD_BGM_FAST)]->CreateStream(String("Resources/sound/music/overworld-bgm-fast.wav"), true);

	sounds[int(SOUND::GAME_PAUSE)] = new FmodSound();
	sounds[int(SOUND::GAME_PAUSE)]->CreateSound(String("Resources/sound/game-pause.wav"));

	sounds[int(SOUND::PLAYER_JUMP)] = new FmodSound();
	sounds[int(SOUND::PLAYER_JUMP)]->CreateSound(String("Resources/sound/player-jump.wav"));

	sounds[int(SOUND::PLAYER_SPIN_JUMP)] = new FmodSound();
	sounds[int(SOUND::PLAYER_SPIN_JUMP)]->CreateSound(String("Resources/sound/player-spin-jump.wav"));

	sounds[int(SOUND::COIN_COLLECT)] = new FmodSound();
	sounds[int(SOUND::COIN_COLLECT)]->CreateSound(String("Resources/sound/coin-collect.wav"));

	sounds[int(SOUND::DRAGON_COIN_COLLECT)] = new FmodSound();
	sounds[int(SOUND::DRAGON_COIN_COLLECT)]->CreateSound(String("Resources/sound/dragon-coin-collect.wav"));

	sounds[int(SOUND::BLOCK_HIT)] = new FmodSound();
	sounds[int(SOUND::BLOCK_HIT)]->CreateSound(String("Resources/sound/block-hit.wav"));

	sounds[int(SOUND::SUPER_MUSHROOM_SPAWN)] = new FmodSound();
	sounds[int(SOUND::SUPER_MUSHROOM_SPAWN)]->CreateSound(String("Resources/sound/super-mushroom-spawn.wav"));
}

void SoundManager::UnloadSounds()
{
	// TODO: Find out if we should be stopping the sounds before we delete them
	for (int i = 0; i < int(SONG::LAST_ELEMENT); ++i)
	{
		delete songs[i];
	}
	for (int i = 0; i < int(SOUND::LAST_ELEMENT); ++i)
	{
		delete sounds[i];
	}
}

void SoundManager::PlaySound(SOUND sound)
{
	if (muted) return;

	sounds[int(sound)]->Play();
}

void SoundManager::SetSoundPaused(SOUND sound, bool paused)
{
	sounds[int(sound)]->SetPaused(paused);
}

void SoundManager::PlaySong(SONG song)
{
	if (muted) return;

	songs[int(song)]->Play();
}

void SoundManager::SetSongPaused(SONG song, bool paused)
{
	songs[int(song)]->SetPaused(paused);
}

void SoundManager::SetVolume(double volume)
{
	globalVolumeLevel = volume;

	for (int i = 0; i < int(SONG::LAST_ELEMENT) - 1; ++i)
	{
		songs[i]->SetVolume(globalVolumeLevel);
	}
	for (int i = 0; i < int(SOUND::LAST_ELEMENT) - 1; ++i)
	{
		sounds[i]->SetVolume(globalVolumeLevel);
	}
}

void SoundManager::SetMuted(bool soundMuted)
{
	if (muted == soundMuted) return;

	muted = soundMuted;
	if (muted) SetVolume(0.0);
	else SetVolume(1.0);
}

void SoundManager::ToggleMuted()
{
	SetMuted(!muted);
}