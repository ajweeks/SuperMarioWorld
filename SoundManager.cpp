#include "stdafx.h"
#include "SoundManager.h"

FmodSound* SoundManager::m_SoundsPtrArr[];
FmodSound* SoundManager::m_SongsPtrArr[];

bool SoundManager::m_Muted = false;
double SoundManager::m_GlobalVolumeLevel = 1.0;

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}

void SoundManager::InitialzeSoundsAndSongs()
{
	// TODO: Only repeat middle section of song, not intro
	m_SongsPtrArr[int(SONG::OVERWORLD_BGM)] = new FmodSound();
	m_SongsPtrArr[int(SONG::OVERWORLD_BGM)]->CreateStream(String("Resources/sound/music/overworld-bgm.wav"), true);

	m_SongsPtrArr[int(SONG::OVERWORLD_BGM_FAST)] = new FmodSound();
	m_SongsPtrArr[int(SONG::OVERWORLD_BGM_FAST)]->CreateStream(String("Resources/sound/music/overworld-bgm-fast.wav"), true);

	m_SoundsPtrArr[int(SOUND::GAME_PAUSE)] = new FmodSound();
	m_SoundsPtrArr[int(SOUND::GAME_PAUSE)]->CreateSound(String("Resources/sound/game-pause.wav"));

	m_SoundsPtrArr[int(SOUND::PLAYER_JUMP)] = new FmodSound();
	m_SoundsPtrArr[int(SOUND::PLAYER_JUMP)]->CreateSound(String("Resources/sound/player-jump.wav"));

	m_SoundsPtrArr[int(SOUND::PLAYER_SPIN_JUMP)] = new FmodSound();
	m_SoundsPtrArr[int(SOUND::PLAYER_SPIN_JUMP)]->CreateSound(String("Resources/sound/player-spin-jump.wav"));

	m_SoundsPtrArr[int(SOUND::PLAYER_SUPER_MUSHROOM_COLLECT)] = new FmodSound();
	m_SoundsPtrArr[int(SOUND::PLAYER_SUPER_MUSHROOM_COLLECT)]->CreateSound(String("Resources/sound/player-super-mushroom-collect.wav"));

	m_SoundsPtrArr[int(SOUND::COIN_COLLECT)] = new FmodSound();
	m_SoundsPtrArr[int(SOUND::COIN_COLLECT)]->CreateSound(String("Resources/sound/coin-collect.wav"));

	m_SoundsPtrArr[int(SOUND::DRAGON_COIN_COLLECT)] = new FmodSound();
	m_SoundsPtrArr[int(SOUND::DRAGON_COIN_COLLECT)]->CreateSound(String("Resources/sound/dragon-coin-collect.wav"));

	m_SoundsPtrArr[int(SOUND::BLOCK_HIT)] = new FmodSound();
	m_SoundsPtrArr[int(SOUND::BLOCK_HIT)]->CreateSound(String("Resources/sound/block-hit.wav"));

	m_SoundsPtrArr[int(SOUND::SUPER_MUSHROOM_SPAWN)] = new FmodSound();
	m_SoundsPtrArr[int(SOUND::SUPER_MUSHROOM_SPAWN)]->CreateSound(String("Resources/sound/super-mushroom-spawn.wav"));

	m_SoundsPtrArr[int(SOUND::MESSAGE_BLOCK_HIT)] = new FmodSound();
	m_SoundsPtrArr[int(SOUND::MESSAGE_BLOCK_HIT)]->CreateSound(String("Resources/sound/message-block-hit.wav"));
}

void SoundManager::RestartSongs()
{
	for (int i = 0; i < int(SONG::LAST_ELEMENT); ++i)
	{
		if (m_SongsPtrArr[i]->IsPlaying())
		{
			m_SongsPtrArr[i]->SetPosition(0);
		}
	}
}

void SoundManager::UnloadSoundsAndSongs()
{
	// TODO: Find out if we should be stopping the sounds before we delete them
	for (int i = 0; i < int(SONG::LAST_ELEMENT); ++i)
	{
		delete m_SongsPtrArr[i];
	}
	for (int i = 0; i < int(SOUND::LAST_ELEMENT); ++i)
	{
		delete m_SoundsPtrArr[i];
	}
}

void SoundManager::PlaySoundEffect(SOUND sound)
{
	if (m_Muted) return;

	m_SoundsPtrArr[int(sound)]->Play();
}

void SoundManager::SetSoundPaused(SOUND sound, bool paused)
{
	m_SoundsPtrArr[int(sound)]->SetPaused(paused);
}

void SoundManager::PlaySong(SONG song)
{
	if (m_Muted) return;

	m_SongsPtrArr[int(song)]->Play();
}

void SoundManager::SetSongPaused(SONG song, bool paused)
{
	m_SongsPtrArr[int(song)]->SetPaused(paused);
}

void SoundManager::SetVolume(double volume)
{
	m_GlobalVolumeLevel = volume;

	for (int i = 0; i < int(SONG::LAST_ELEMENT) - 1; ++i)
	{
		m_SongsPtrArr[i]->SetVolume(m_GlobalVolumeLevel);
	}
	for (int i = 0; i < int(SOUND::LAST_ELEMENT) - 1; ++i)
	{
		m_SoundsPtrArr[i]->SetVolume(m_GlobalVolumeLevel);
	}
}

void SoundManager::SetMuted(bool soundMuted)
{
	if (m_Muted == soundMuted) return;

	m_Muted = soundMuted;
	if (m_Muted) SetVolume(0.0);
	else SetVolume(1.0);
}

void SoundManager::ToggleMuted()
{
	SetMuted(!m_Muted);
}