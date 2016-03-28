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

	void PaintHUD();
	unsigned int GetNumberOfDigits(unsigned int i);
	void PaintSeveralDigitNumber(int x, int y, int number, bool yellow);
	RECT2 GetSmallSingleNumberSrcRect(int number, bool yellow);

	void ReadLevelData(int levelIndex);
	LevelData* m_LevelDataPtr = nullptr;

	DOUBLE2 m_NewCoinPos = DOUBLE2();
	Item* m_ItemToBeRemoved = nullptr;

	PhysicsActor* m_ActLevelPtr = nullptr;

	int m_TotalTime;
	double m_SecondsElapsed = 0.0;

	double m_Width;
	double m_Height;

	// TODO(AJ): Rename this to Mario? What about Luigi? Add player 2?
	Player *m_PlayerPtr = nullptr;
	bool m_IsPlayerOnGround = false;

	Camera* m_Camera = nullptr;
};
