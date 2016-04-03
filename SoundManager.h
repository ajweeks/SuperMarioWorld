#pragma once

class SoundManager
{
public:
	enum class SOUND
	{
		GAME_PAUSE, PLAYER_JUMP, PLAYER_SPIN_JUMP, COIN_COLLECT, DRAGON_COIN_COLLECT,
		BLOCK_HIT, SUPER_MUSHROOM_SPAWN,
		// NOTE: All entries must be above this line
		LAST_ELEMENT
	};
	enum class SONG 
	{
		OVERWORLD_BGM, OVERWORLD_BGM_FAST,
		// NOTE: All entries must be above this line
		LAST_ELEMENT
	};

	virtual ~SoundManager();

	SoundManager(const SoundManager&) = delete;
	SoundManager& operator=(const SoundManager&) = delete;

	static void InitialzeSounds();
	static void UnloadSounds();

	static void PlaySound(SOUND sound);
	static void SetSoundPaused(SOUND sound, bool paused);
	static void PlaySong(SONG song);
	static void SetSongPaused(SONG song, bool paused);

	static void SetVolume(double volume);
	static void SetMuted(bool muted);
	static void ToggleMuted();

private:
	SoundManager();
	
	static FmodSound* sounds[int(SOUND::LAST_ELEMENT)];
	static FmodSound* songs[int(SONG::LAST_ELEMENT)];

	static double globalVolumeLevel;
	static bool muted;

};

