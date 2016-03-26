#pragma once

#include "Player.h"
#include "ContactListener.h"

class LevelData;
struct Platform;
struct Pipe;
struct Item;

class Camera;

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

	double GetWidth();
	double GetHeight();

	bool IsOnGround(PhysicsActor *otherActPtr);

	void TogglePaused(bool paused);

private:
	void PreSolve(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr, bool & enableContactRef);
	void BeginContact(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr);
	void EndContact(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr);

	void DEBUGPaintZoomedOut();

	void ReadLevelData(int levelIndex);
	LevelData* m_LevelDataPtr = nullptr;

	PhysicsActor* m_ActLevelPtr = nullptr;
	//std::vector<Platform*> m_PlatformsPtrArr;
	//std::vector<Pipe*> m_PipesPtrArr;
	//std::vector<Item*> m_ItemsPtrArr;

	double m_Width;
	double m_Height;

	// TODO(AJ): Rename this to Mario? What about Luigi? Add player 2?
	Player *m_PlayerPtr = nullptr;
	bool m_IsPlayerOnGround = false;

	Camera* m_Camera = nullptr;
};
