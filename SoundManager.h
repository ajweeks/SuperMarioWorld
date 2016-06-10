#pragma once

class SoundManager
{
public:
	enum class Sound
	{
		GAME_PAUSE, PSWITCH_ACTIVATE, PSWITCH_TIME_WARNING, TIME_WARNING,
		PLAYER_JUMP, PLAYER_SPIN_JUMP, PLAYER_SUPER_MUSHROOM_COLLECT, PLAYER_DEATH, PLAYER_DAMAGE, PLAYER_ONE_UP,
		COIN_COLLECT, DRAGON_COIN_COLLECT, BLOCK_HIT, BLOCK_BREAK, SUPER_MUSHROOM_SPAWN, MESSAGE_BLOCK_HIT,
		FIRE_BALL_THROW, CHARGIN_CHUCK_HEAD_BONK, CHARGIN_CHUCK_TAKE_DAMAGE, SHELL_KICK,
		ENEMY_HEAD_STOMP_START, ENEMY_HEAD_STOMP_END, MIDWAY_GATE_PASSTHROUGH, BEANSTALK_SPAWN, 
		COURSE_CLEAR_FANFARE, DRUMROLL, OUTRO_CIRCLE_TRANSITION, LEVEL_SELECT_NODE_STOMP,
		YOSHI_SPAWN, YOSHI_EGG_BREAK, YOSHI_PLAYER_MOUNT, YOSHI_FIRE_SPIT, YOSHI_SPIT, YOSHI_SWALLOW, YOSHI_TOUNGE_OUT, YOSHI_RUN_AWAY,
		// NOTE: All entries must be above this line
		_LAST_ELEMENT
	};
	enum class Song 
	{
		OVERWORLD_BGM, OVERWORLD_BGM_FAST, 
		UNDERGROUND_BGM, UNDERGROUND_BGM_FAST, 
		MENU_SCREEN_BGM, MAP1_YOSHIS_ISLAND,
		CHARGIN_CHUCK_RUN,
		// NOTE: All entries must be above this line
		_LAST_ELEMENT
	};

	virtual ~SoundManager();

	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;

	static void InitialzeSoundsAndSongs();
	static void UnloadSoundsAndSongs();

	static void PlaySoundEffect(Sound sound);
	//static void SetSoundPaused(Sound sound, bool paused); // UNUSED
	static void PlaySong(Song song);
	static void SetSongPaused(Song song, bool paused);

	static void RestartSongs();

	static void SetMuted(bool muted);
	static void ToggleMuted();
	
	static bool IsMuted();
	static bool IsInitialized();

	static void SetAllSongsPaused(bool paused);

private:
	SoundManager();

	static void LoadSong(Song song, String filePath);
	static void LoadSound(Sound sound, String filePath);

	static void SetVolume(double volume);

	static bool m_IsInitialized;

	static const String m_ResourcePath;

	static FmodSound* m_SoundsPtrArr[int(Sound::_LAST_ELEMENT)];
	static FmodSound* m_SongsPtrArr[int(Song::_LAST_ELEMENT)];

	static double m_GlobalVolumeLevel;
	static bool m_Muted;

};
