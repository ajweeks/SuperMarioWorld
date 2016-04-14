#pragma once

#include "ContactListener.h"

class LevelData;
class Platform;
class Pipe;
class Item;
class Enemy;
class Player;

class Camera;
class Particle;
class ParticleManager;
class SpriteSheet;

class Level : public ContactListener
{
public:
	Level();
	virtual ~Level();

	Level& operator=(const Level&) = delete;
	Level(const Level&) = delete;

	void Tick(double deltaTime);
	void Paint();

	void Reset();

	void AddItem(Item* newItemPtr);
	void RemoveItem(Item* itemPtr);

	void AddEnemy(Enemy* newEnemyPtr);
	void RemoveEnemy(Enemy* enemyPtr);

	double GetWidth();
	double GetHeight();

	bool IsPlayerOnGround();

	void SetShowingMessage(bool showingMessage);
	bool IsShowingMessage();

	void SetPaused(bool paused);

	Player* GetPlayer();
	void GiveItemToPlayer(Item* itemPtr);

	DOUBLE2 GetCameraOffset();

	void AddParticle(Particle* particlePtr);

	static const int COLLIDE_WITH_LEVEL		= 0x0001;
	static const int COLLIDE_WITH_ENEMIES	= 0x0002;
	static const int COLLIDE_WITH_PLAYER	= 0x0004;
	bool Raycast(DOUBLE2 point1, DOUBLE2 point2, int collisionBits, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef);

private:
	void PreSolve(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr, bool & enableContactRef);
	void BeginContact(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr);
	void EndContact(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr);
	bool ActorCanPassThroughPlatform(PhysicsActor *actPlatformPtr, DOUBLE2& actorPosRef, double actorWidth, double actorHeight);

	void DEBUGPaintZoomedOut();

	void PaintHUD();
	unsigned int GetNumberOfDigits(unsigned int i);

	void TogglePaused();

	void ReadLevelData(int levelIndex);
	LevelData* m_LevelDataPtr = nullptr;

	// NOTE: We *could* make this an array, in case the player tries to collect two items
	// in the same frame, but that isn't very likely and will be caught in the next frame
	// anyways, so this should be just fine
	Item* m_ItemToBeRemovedPtr = nullptr;

	PhysicsActor* m_ActLevelPtr = nullptr;

	bool m_Paused = false;

	bool m_ShowingMessage = false;

	int m_TotalTime; // How long the player has to complete this level
	double m_SecondsElapsed = 0.0; // How many real-time seconds have elapsed

	// How many pixels wide the foreground of the level is
	double m_Width;
	// How many pixels high the foreground of the level is
	double m_Height;

	// TODO: Rename this to Mario? What about Luigi? Add player 2?
	Player *m_PlayerPtr = nullptr;
	bool m_IsPlayerOnGround = false;

	Camera* m_CameraPtr = nullptr;

	ParticleManager* m_ParticleManagerPtr = nullptr;
};

// TODO: Move this somewhere more global
void PaintSeveralDigitNumber(int x, int y, int number, bool yellow);
void PaintSeveralDigitLargeNumber(int x, int y, int number);
RECT2 GetSmallSingleNumberSrcRect(int number, bool yellow);
RECT2 GetLargeSingleNumberSrcRect(int number);