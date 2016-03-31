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

	bool IsPlayerOnGround();

	//bool IsPaused();

private:
	void PreSolve(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr, bool & enableContactRef);
	void BeginContact(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr);
	void EndContact(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr);

	void DEBUGPaintZoomedOut();

	void PaintHUD();
	unsigned int GetNumberOfDigits(unsigned int i);
	void PaintSeveralDigitNumber(int x, int y, int number, bool yellow);
	void PaintSeveralDigitLargeNumber(int x, int y, int number);
	RECT2 GetSmallSingleNumberSrcRect(int number, bool yellow);
	RECT2 GetLargeSingleNumberSrcRect(int number);

	void ReadLevelData(int levelIndex);
	LevelData* m_LevelDataPtr = nullptr;

	void TogglePaused(bool paused);

	DOUBLE2 m_NewCoinPos = DOUBLE2();
	Item* m_ItemToBeRemoved = nullptr;

	PhysicsActor* m_ActLevelPtr = nullptr;

	bool m_Paused = false;
	// NOTE: This stupid variable is only here because
	// Box2D won't let us set all the actors to inactive during a PreSolve OR a BeginContact
	// so we need to save the state and update it next tick instead....
	bool m_WasPaused = false; 

	int m_TotalTime; // How long the player has to complete this level
	double m_SecondsElapsed = 0.0; // How many real-time seconds have elapsed

	// How many pixels wide the foreground of the level is
	double m_Width;
	// How many pixels high the foreground of the level is
	double m_Height;

	// TODO(AJ): Rename this to Mario? What about Luigi? Add player 2?
	Player *m_PlayerPtr = nullptr;
	bool m_IsPlayerOnGround = false;

	Camera* m_Camera = nullptr;
};
