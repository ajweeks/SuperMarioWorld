#pragma once

#include "ContactListener.h"
#include "Enumerations.h"
#include "CountdownTimer.h"

class Game;
class LevelData;
class Platform;
class Pipe;
class Item;
class Enemy;
class Player;
class Yoshi;

class Camera;
class Particle;
class ParticleManager;
class SpriteSheet;

class Level : public ContactListener
{
public:
	Level(Game* gamePtr);
	virtual ~Level();

	Level(const Level&) = delete;
	Level& operator=(const Level&) = delete;

	void Tick(double deltaTime);
	void Paint();

	void Reset();

	void AddItem(Item* newItemPtr);
	void RemoveItem(Item* itemPtr);

	void AddEnemy(Enemy* newEnemyPtr);
	void RemoveEnemy(Enemy* enemyPtr);

	void AddYoshi(Yoshi* yoshiPtr);

	double GetWidth();
	double GetHeight();

	int GetTimeRemaining();

	void SetShowingMessage(bool showingMessage);
	bool IsShowingMessage();

	void SetPaused(bool paused);

	bool IsCheckpointCleared();

	void SetAllDragonCoinsCollected(bool allCollected);
	bool AllDragonCoinsCollected();

	Player* GetPlayer();
	void GiveItemToPlayer(Item* itemPtr);

	DOUBLE2 GetCameraOffset();

	void AddParticle(Particle* particlePtr);

	static const int COLLIDE_WITH_LEVEL		= (2 << 0);
	static const int COLLIDE_WITH_ENEMIES	= (2 << 1);
	static const int COLLIDE_WITH_PLAYER	= (2 << 2);
	static const int COLLIDE_WITH_BLOCKS    = (2 << 3);
	static const int COLLIDE_WITH_SHELLS	= (2 << 4);
	bool Raycast(DOUBLE2 point1, DOUBLE2 point2, int collisionBits, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef);

	void TriggerEndScreen();

	void SetPausedTimer(int duration);

private:
	void PreSolve(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr, bool & enableContactRef);
	void BeginContact(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr);
	void EndContact(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr);
	bool ActorCanPassThroughPlatform(PhysicsActor *actPlatformPtr, DOUBLE2& actorPosRef, double actorWidth, double actorHeight);

	void DEBUGPaintZoomedOut();
	
	void ResetMembers();

	void PaintHUD();
	unsigned int GetNumberOfDigits(unsigned int i);

	void TogglePaused();

	static const int TIME_WARNING = 100; // When this many in game seconds are remaining the player is notified

	void ReadLevelData(int levelIndex);
	LevelData* m_LevelDataPtr = nullptr;

	Game* m_GamePtr = nullptr;

	// NOTE: We *could* make this an array, in case the player tries to collect two items
	// in the same frame, but that isn't very likely and will be caught in the next frame
	// anyways, so this should be just fine
	std::vector<Item*> m_ItemsToBeRemovedPtrArr;

	PhysicsActor* m_ActLevelPtr = nullptr;

	bool m_Paused;

	bool m_ShowingMessage;

	int m_TotalTime; // How long the player has to complete this level
	double m_SecondsElapsed; // How many real-time seconds have elapsed
	int m_TimeRemaining;

	// How many pixels wide the foreground of the level is
	double m_Width;
	// How many pixels high the foreground of the level is
	double m_Height;

	bool m_AllDragonCoinsCollected;

	bool m_IsCheckpointCleared;
	
	// This can be set by various classes to pause the game for a short duration
	// (eg. when the player changes powerup states, when a message block is animating in or out, etc.)
	CountdownTimer m_GamePausedTimer;

	Player *m_PlayerPtr = nullptr;
	Camera* m_CameraPtr = nullptr;
	ParticleManager* m_ParticleManagerPtr = nullptr;
	Yoshi* m_YoshiPtr = nullptr;

	bool m_IsShowingEndScreen;
	int m_FramesShowingEndScreen;
};
