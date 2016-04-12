#include "stdafx.h"
#include "SoundManager.h"

FmodSound* SoundManager::m_SoundsPtrArr[];
FmodSound* SoundManager::m_SongsPtrArr[];

bool SoundManager::m_Muted = false;
double SoundManager::m_GlobalVolumeLevel = 1.0;

const String SoundManager::m_ResourcePath = String("Resources/sound/");

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}

void SoundManager::InitialzeSoundsAndSongs()
{
	// TODO: Only repeat middle section of song, not intro
	LoadSong(SONG::OVERWORLD_BGM, m_ResourcePath + String("music/overworld-bgm.wav"));
	LoadSong(SONG::OVERWORLD_BGM_FAST, m_ResourcePath + String("music/overworld-bgm-fast.wav"));

	LoadSound(SOUND::GAME_PAUSE, m_ResourcePath + String("game-pause.wav"));

	LoadSound(SOUND::PLAYER_JUMP, m_ResourcePath + String("player-jump.wav"));
	LoadSound(SOUND::PLAYER_SPIN_JUMP, m_ResourcePath + String("player-spin-jump.wav"));
	LoadSound(SOUND::PLAYER_SUPER_MUSHROOM_COLLECT, m_ResourcePath + String("player-super-mushroom-collect.wav"));
	LoadSound(SOUND::PLAYER_DEATH, m_ResourcePath + String("player-death.wav"));

	LoadSound(SOUND::COIN_COLLECT, m_ResourcePath + String("coin-collect.wav"));
	LoadSound(SOUND::DRAGON_COIN_COLLECT, m_ResourcePath + String("dragon-coin-collect.wav"));
	LoadSound(SOUND::BLOCK_HIT, m_ResourcePath + String("block-hit.wav"));
	LoadSound(SOUND::BLOCK_BREAK, m_ResourcePath + String("block-break.wav"));

	LoadSound(SOUND::SUPER_MUSHROOM_SPAWN, m_ResourcePath + String("super-mushroom-spawn.wav"));
	LoadSound(SOUND::MESSAGE_BLOCK_HIT, m_ResourcePath + String("message-block-hit.wav"));

	// TODO: Add pitch variation to this sound based on how many enemies a shell has hit in a row
	LoadSound(SOUND::KOOPA_DEATH, m_ResourcePath + String("koopa-death.wav"));
	LoadSound(SOUND::ENEMY_HEAD_STOMP_START, m_ResourcePath + String("enemy-head-stomp-start.wav"));
	LoadSound(SOUND::ENEMY_HEAD_STOMP_END, m_ResourcePath + String("enemy-head-stomp-end.wav"));
}

void SoundManager::LoadSong(SONG song, String filePath)
{
	assert(int(song) >= 0 && int(song) < int(SONG::_LAST_ELEMENT));
	assert(m_SongsPtrArr[int(song)] == nullptr);

	m_SongsPtrArr[int(song)] = new FmodSound();
	m_SongsPtrArr[int(song)]->CreateStream(filePath, true);
}

void SoundManager::LoadSound(SOUND sound, String filePath)
{
	assert(int(sound) >= 0 && int(sound) < int(SOUND::_LAST_ELEMENT));
	assert(m_SoundsPtrArr[int(sound)] == nullptr);

	m_SoundsPtrArr[int(sound)] = new FmodSound();
	m_SoundsPtrArr[int(sound)]->CreateSound(filePath);
}

void SoundManager::RestartSongs()
{
	for (int i = 0; i < int(SONG::_LAST_ELEMENT); ++i)
	{
		if (m_SongsPtrArr[i]->IsPlaying())
		{
			m_SongsPtrArr[i]->SetPaused(true);
			m_SongsPtrArr[i]->SetPosition(0);
		}
	}
}

void SoundManager::UnloadSoundsAndSongs()
{
	// TODO: Find out if we should be stopping the sounds before we delete them
	for (int i = 0; i < int(SONG::_LAST_ELEMENT); ++i)
	{
		delete m_SongsPtrArr[i];
	}
	for (int i = 0; i < int(SOUND::_LAST_ELEMENT); ++i)
	{
		delete m_SoundsPtrArr[i];
	}
}

void SoundManager::PlaySoundEffect(SOUND sound)
{
	if (m_Muted) return;

	m_SoundsPtrArr[int(sound)]->Play();
}

void SoundManager::SetAllSongsPaused(bool paused)
{
	for (size_t i = 0; i < int(SONG::_LAST_ELEMENT); ++i)
	{
		SetSongPaused(SONG(i), paused);
	}
}

void SoundManager::SetSoundPaused(SOUND sound, bool paused)
{
	m_SoundsPtrArr[int(sound)]->SetPaused(paused);
}

void SoundManager::PlaySong(SONG song)
{
	m_SongsPtrArr[int(song)]->Play();
	m_SongsPtrArr[int(song)]->SetVolume(m_GlobalVolumeLevel);
}

void SoundManager::SetSongPaused(SONG song, bool paused)
{
	m_SongsPtrArr[int(song)]->SetPaused(paused);
}

void SoundManager::SetVolume(double volume)
{
	m_GlobalVolumeLevel = volume;
	if (volume > 0.0)
	{
		m_Muted = false;
	}

	for (int i = 0; i < int(SONG::_LAST_ELEMENT) - 1; ++i)
	{
		m_SongsPtrArr[i]->SetVolume(m_GlobalVolumeLevel);
	}
	for (int i = 0; i < int(SOUND::_LAST_ELEMENT) - 1; ++i)
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

bool SoundManager::IsMuted()
{
	return m_Muted;
}