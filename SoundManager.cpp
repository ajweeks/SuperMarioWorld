#include "stdafx.h"
#include "SoundManager.h"

FmodSound* SoundManager::m_SoundsPtrArr[];
FmodSound* SoundManager::m_SongsPtrArr[];

bool SoundManager::m_Muted = false;
double SoundManager::m_GlobalVolumeLevel = 1.0;

const String SoundManager::m_ResourcePath = String("Resources/sound/");

bool SoundManager::m_IsInitialized = false;

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}

void SoundManager::InitialzeSoundsAndSongs()
{
	// TODO: Only repeat middle section of song, not intro
	// SONGS
	LoadSong(Song::OVERWORLD_BGM, m_ResourcePath + String("music/overworld-bgm.wav"));
	LoadSong(Song::OVERWORLD_BGM_FAST, m_ResourcePath + String("music/overworld-bgm-fast.wav"));
	LoadSong(Song::UNDERGROUND_BGM, m_ResourcePath + String("music/underground-bgm.wav"));
	LoadSong(Song::UNDERGROUND_BGM_FAST, m_ResourcePath + String("music/underground-bgm-fast.wav"));
	LoadSong(Song::MENU_SCREEN_BGM, m_ResourcePath + String("music/menu-screen-bgm.wav"));
	LoadSong(Song::MAP1_YOSHIS_ISLAND, m_ResourcePath + String("music/map-1-yoshis-island.wav"));

	// SOUNDS
	LoadSound(Sound::GAME_PAUSE, m_ResourcePath + String("game-pause.wav"));
	LoadSound(Sound::TIME_WARNING, m_ResourcePath + String("time-warning.wav"));
	LoadSound(Sound::PSWITCH_ACTIVATE, m_ResourcePath + String("pswitch-activate.wav"));
	LoadSound(Sound::PSWITCH_TIME_WARNING, m_ResourcePath + String("pswitch-time-warning.wav"));

	LoadSound(Sound::PLAYER_JUMP, m_ResourcePath + String("player-jump.wav"));
	LoadSound(Sound::PLAYER_SPIN_JUMP, m_ResourcePath + String("player-spin-jump.wav"));
	LoadSound(Sound::PLAYER_SUPER_MUSHROOM_COLLECT, m_ResourcePath + String("player-super-mushroom-collect.wav"));
	LoadSound(Sound::PLAYER_DEATH, m_ResourcePath + String("player-death.wav"));
	LoadSound(Sound::PLAYER_DAMAGE, m_ResourcePath + String("player-damage+pipe-enter.wav"));
	LoadSound(Sound::PLAYER_ONE_UP, m_ResourcePath + String("one-up.wav"));

	LoadSound(Sound::COIN_COLLECT, m_ResourcePath + String("coin-collect.wav"));
	LoadSound(Sound::DRAGON_COIN_COLLECT, m_ResourcePath + String("dragon-coin-collect.wav"));
	LoadSound(Sound::BLOCK_HIT, m_ResourcePath + String("block-hit.wav"));
	LoadSound(Sound::BLOCK_BREAK, m_ResourcePath + String("block-break.wav"));

	LoadSound(Sound::FIRE_BALL_THROW, m_ResourcePath + String("fire-ball-throw.wav"));
	LoadSound(Sound::CHARGIN_CHUCK_HEAD_BONK, m_ResourcePath + String("chargin-chuck-head-bonk.wav"));
	LoadSong(Song::CHARGIN_CHUCK_RUN, m_ResourcePath + String("chargin-chuck-run.wav"));
	LoadSound(Sound::CHARGIN_CHUCK_TAKE_DAMAGE, m_ResourcePath + String("chargin-chuck-take-damage.wav"));
	LoadSound(Sound::BEANSTALK_SPAWN, m_ResourcePath + String("beanstalk-spawn.wav"));
	LoadSound(Sound::SUPER_MUSHROOM_SPAWN, m_ResourcePath + String("super-mushroom-spawn.wav"));
	LoadSound(Sound::MESSAGE_BLOCK_HIT, m_ResourcePath + String("message-block-hit.wav"));
	LoadSound(Sound::SHELL_KICK, m_ResourcePath + String("shell-kick.wav"));

	// TODO: Add pitch variation to this sound based on how many enemies a shell has hit in a row
	LoadSound(Sound::ENEMY_HEAD_STOMP_START, m_ResourcePath + String("enemy-head-stomp-start.wav"));
	LoadSound(Sound::ENEMY_HEAD_STOMP_END, m_ResourcePath + String("enemy-head-stomp-end.wav"));

	LoadSound(Sound::MIDWAY_GATE_PASSTHROUGH, m_ResourcePath + String("midway-gate-passthrough.wav"));
	LoadSound(Sound::COURSE_CLEAR_FANFARE, m_ResourcePath + String("course-clear-fanfare.wav"));
	LoadSound(Sound::DRUMROLL, m_ResourcePath + String("outro-drumroll.wav"));
	LoadSound(Sound::OUTRO_CIRCLE_TRANSITION, m_ResourcePath + String("outro-circle-transition.wav"));
	LoadSound(Sound::MAP_NEW_LOCATION_STOMP, m_ResourcePath + String("map-new-location-stomp.wav"));

	LoadSound(Sound::YOSHI_SPAWN, m_ResourcePath + String("yoshi-spawn.wav"));
	LoadSound(Sound::YOSHI_EGG_BREAK, m_ResourcePath + String("yoshi-egg-break.wav"));
	LoadSound(Sound::YOSHI_FIRE_SPIT, m_ResourcePath + String("yoshi-fire-spit.wav"));
	LoadSound(Sound::YOSHI_SPIT, m_ResourcePath + String("yoshi-spit.wav"));
	LoadSound(Sound::YOSHI_PLAYER_MOUNT, m_ResourcePath + String("yoshi-player-mount.wav"));
	LoadSound(Sound::YOSHI_RUN_AWAY, m_ResourcePath + String("yoshi-run-away.wav"));
	LoadSound(Sound::YOSHI_SWALLOW, m_ResourcePath + String("yoshi-swallow.wav"));
	LoadSound(Sound::YOSHI_TOUNGE_OUT, m_ResourcePath + String("yoshi-tounge-out.wav"));

	m_IsInitialized = true;
}

void SoundManager::LoadSong(Song song, String filePath)
{
	assert(int(song) >= 0 && int(song) < int(Song::_LAST_ELEMENT));
	assert(m_SongsPtrArr[int(song)] == nullptr);

	m_SongsPtrArr[int(song)] = new FmodSound();
	m_SongsPtrArr[int(song)]->CreateStream(filePath, true);
}

void SoundManager::LoadSound(Sound sound, String filePath)
{
	assert(int(sound) >= 0 && int(sound) < int(Sound::_LAST_ELEMENT));
	assert(m_SoundsPtrArr[int(sound)] == nullptr);

	m_SoundsPtrArr[int(sound)] = new FmodSound();
	// NOTE: The game engine doesn't seem to use handle Sounds correctly, just use streams instead
	m_SoundsPtrArr[int(sound)]->CreateStream(filePath, false);
}

void SoundManager::RestartSongs()
{
	for (int i = 0; i < int(Song::_LAST_ELEMENT); ++i)
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
	for (int i = 0; i < int(Song::_LAST_ELEMENT); ++i)
	{
		delete m_SongsPtrArr[i];
		m_SongsPtrArr[i] = nullptr;
	}
	for (int i = 0; i < int(Sound::_LAST_ELEMENT); ++i)
	{
		delete m_SoundsPtrArr[i];
		m_SoundsPtrArr[i] = nullptr;
	}
}

void SoundManager::PlaySoundEffect(Sound sound)
{
	if (m_Muted) return;

	//if (m_SoundsPtrArr[int(sound)]->IsPlaying()) 
	//	m_SoundsPtrArr[int(sound)]->SetPosition(0);
	//else
		m_SoundsPtrArr[int(sound)]->Play();
}

void SoundManager::SetAllSongsPaused(bool paused)
{
	for (size_t i = 0; i < int(Song::_LAST_ELEMENT); ++i)
	{
		SetSongPaused(Song(i), paused);
	}
}

//void SoundManager::SetSoundPaused(Sound sound, bool paused)
//{
//	if (m_SoundsPtrArr[int(sound)] == nullptr) return;
//	m_SoundsPtrArr[int(sound)]->SetPaused(paused);
//}

void SoundManager::PlaySong(Song song)
{
	m_SongsPtrArr[int(song)]->Play();
	m_SongsPtrArr[int(song)]->SetVolume(m_GlobalVolumeLevel);
}

void SoundManager::SetSongPaused(Song song, bool paused)
{
	if (m_SongsPtrArr[int(song)] == nullptr) return;
	m_SongsPtrArr[int(song)]->SetPaused(paused);
}

void SoundManager::SetVolume(double volume)
{
	m_GlobalVolumeLevel = volume;
	if (volume > 0.0)
	{
		m_Muted = false;
	}

	for (int i = 0; i < int(Song::_LAST_ELEMENT) - 1; ++i)
	{
		if (m_SongsPtrArr[i] != nullptr)
		{
			m_SongsPtrArr[i]->SetVolume(m_GlobalVolumeLevel);
		}
	}
	for (int i = 0; i < int(Sound::_LAST_ELEMENT) - 1; ++i)
	{
		if (m_SoundsPtrArr[i] != nullptr)
		{
			m_SoundsPtrArr[i]->SetVolume(m_GlobalVolumeLevel);
		}
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

bool SoundManager::IsInitialized()
{
	return m_IsInitialized;
}
