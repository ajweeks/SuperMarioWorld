#pragma once

class SoundManager
{
public:
	enum class SOUND
	{
		GAME_PAUSE, PLAYER_JUMP, PLAYER_SPIN_JUMP, PLAYER_SUPER_MUSHROOM_COLLECT, PLAYER_DEATH,
		COIN_COLLECT, DRAGON_COIN_COLLECT, BLOCK_HIT, SUPER_MUSHROOM_SPAWN, MESSAGE_BLOCK_HIT,
		KOOPA_DEATH, ENEMY_HEAD_STOMP_START, ENEMY_HEAD_STOMP_END,
		// NOTE: All entries must be above this line
		_LAST_ELEMENT
	};
	enum class SONG 
	{
		OVERWORLD_BGM, OVERWORLD_BGM_FAST,
		// NOTE: All entries must be above this line
		_LAST_ELEMENT
	};

	virtual ~SoundManager();

	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;

	static void InitialzeSoundsAndSongs();
	static void UnloadSoundsAndSongs();

	static void PlaySoundEffect(SOUND sound);
	static void SetSoundPaused(SOUND sound, bool paused);
	static void PlaySong(SONG song);
	static void SetSongPaused(SONG song, bool paused);

	static void RestartSongs();

	static void SetMuted(bool muted);
	static void ToggleMuted();
	
	static bool IsMuted();

	static void SetAllSongsPaused(bool paused);

private:
	SoundManager();

	static void LoadSong(SONG song, String filePath);
	static void LoadSound(SOUND sound, String filePath);

	static void SetVolume(double volume);

	static const String m_ResourcePath;

	static FmodSound* m_SoundsPtrArr[int(SOUND::_LAST_ELEMENT)];
	static FmodSound* m_SongsPtrArr[int(SONG::_LAST_ELEMENT)];

	static double m_GlobalVolumeLevel;
	static bool m_Muted;

};

