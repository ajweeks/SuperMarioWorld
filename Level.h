#pragma once

#include "ContactListener.h"
#include "Enumerations.h"
#include "SMWTimer.h"
#include "SoundManager.h"
#include "AnimationInfo.h"
#include "SessionInfo.h"

class Game;
class GameState;
class LevelData;
class Platform;
class Pipe;
class Item;
class Enemy;
class Player;
class Yoshi;
class Message;

class Camera;
class Particle;
class ParticleManager;
class SpriteSheet;

struct LevelProperties;

class Level : public ContactListener
{
public:
	enum CollisionFilter
	{
		LEVEL			= (1 << 0),
		ENEMY			= (1 << 1),
		PLAYER			= (1 << 2),
		BLOCK			= (1 << 3),
		SHELL			= (1 << 4),
		YOSHI			= (1 << 5),
		YOSHI_TOUNGE	= (1 << 6),
		BERRY			= (1 << 7),
		ITEM			= (1 << 8),
		BEANSTALK		= (1 << 9),
		FIREBALL		= (1 << 10),
	};

	// Used to caculate/show the player's extra score at the end of the level
	struct FinalExtraScore
	{
		// How much time was remaining when the player crossed through the goal gate
		int m_FinalTimeRemaining;
		// This value is set to final time * 50, then gradually is "moved" to the player's score value (100 pts, per frame)
		int m_ScoreShowing;
		// The higher the bar is when the player hits it, the larger this value is (-1 if they don't hit it)
		int m_BonusScoreShowing;
	};

	Level(Game* gamePtr, GameState* gameStatePtr, LevelProperties levelInfo, SessionInfo sessionInfo = {}, Pipe* spawningPipePtr = nullptr);
	virtual ~Level();

	Level(const Level&) = delete;
	Level& operator=(const Level&) = delete;

	void Tick(double deltaTime);
	void Paint();
	void Reset();

	void AddItem(Item* newItemPtr, bool addContactListener = false);
	void RemoveItem(Item* itemPtr);
	void AddEnemy(Enemy* newEnemyPtr);
	void RemoveEnemy(Enemy* enemyPtr);
	void AddYoshi(Yoshi* yoshiPtr);
	Yoshi* GetYoshiPtr();

	double GetWidth() const;
	double GetHeight() const;
	int GetTimeRemaining() const;
	void SetPaused(bool paused, bool pauseSongs);
	void SetPausedTimer(int duration, bool pauseSongs);
	bool IsPaused() const;

	bool IsCheckpointCleared() const;
	Player* GetPlayer() const;
	DOUBLE2 GetCameraOffset(double deltaTime);
	void SpeedUpMusic();

	void SetActiveMessage(Message* activeMessagePtr);
	void WarpPlayerToPipe(int pipeIndex);

	void GiveItemToPlayer(Item* itemPtr);
	void RemoveParticle(Particle* particlePtr);
	void AddParticle(Particle* particlePtr);
	bool Raycast(DOUBLE2 point1, DOUBLE2 point2, int collisionBits, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef);
	void TriggerEndScreen(int barHitHeight = -1);

private:
	void PreSolve(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr, bool & enableContactRef);
	void BeginContact(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr);
	void EndContact(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr);

	bool ActorCanPassThroughPlatform(PhysicsActor *actPlatformPtr, DOUBLE2& actorPosRef, double actorWidth, double actorHeight);
	void CollidePlayerWithBlock(DOUBLE2 blockCenterPos, DOUBLE2 playerFeet, bool& enableContactRef);

	void DEBUGPaintZoomedOut();
	void ResetMembers();
	unsigned int GetNumberOfDigits(unsigned int i) const;
	void TogglePaused(bool pauseSongs);
	void TurnCoinsToBlocks(bool toBlocks);
	void ReadLevelData(int levelIndex);
	void PaintHUD();
	void PaintEnclosingCircle(DOUBLE2 circleCenter, double innerCircleRadius);

	static const double TIME_SCALE; // How fast an in-game second is compared to a real life second
	static const int TIME_UP_WARNING; // When this many in game seconds are remaining a sound is played
	static const int MESSAGE_BLOCK_WARNING_TIME; // Play a warning sound when this many frames are remaining in the pressed timer

	const int INDEX;

	bool m_IsShowingEndScreen;
	FinalExtraScore m_FinalExtraScore;

	GameState* m_GameStatePtr = nullptr;
	LevelData* m_LevelDataPtr = nullptr;
	SoundManager::Song m_BackgroundSong;
	SoundManager::Song m_BackgroundSongFast;

	Game* m_GamePtr = nullptr;

	const bool IS_BACKGROUND_ANIMATED;
	const int TOTAL_FRAMES_OF_BACKGROUND_ANIMATION;
	AnimationInfo m_BackgroundAnimInfo;

	Bitmap* m_BmpForegroundPtr = nullptr;
	Bitmap* m_BmpBackgroundPtr = nullptr;

	std::vector<Item*> m_ItemsToBeRemovedPtrArr;
	std::vector<Enemy*> m_EnemiesToBeRemovedPtrArr;

	PhysicsActor* m_ActLevelPtr = nullptr;

	bool m_TimeWarningPlayed;
	bool m_PSwitchTimeWarningPlayed;
	bool m_Paused;

	Message* m_ActiveMessagePtr = nullptr;

	double m_SecondsElapsed; // How many real-time seconds have elapsed
	
	const int TOTAL_TIME; // How many in-game seconds the player has to complete this level
	int m_TimeRemaining; // How many in-game seconds are remaining
	
	// How many pixels wide the foreground of the level is
	const double WIDTH;
	// How many pixels high the foreground of the level is
	const double HEIGHT;
	
	bool m_IsCheckpointCleared;
	
	// This can be set by various classes to pause the game for a short duration
	// (eg. when the player changes powerup states, when a message block is animating in or out, etc.)
	SMWTimer m_GamePausedTimer;
	SMWTimer m_CoinsToBlocksTimer;

	Player *m_PlayerPtr = nullptr;
	Camera* m_CameraPtr = nullptr;
	ParticleManager* m_ParticleManagerPtr = nullptr;
	Yoshi* m_YoshiPtr = nullptr;
};
