#pragma once

#include "Item.h"

class Fireball : public Item
{
public:
	Fireball(DOUBLE2 topLeft, Level* m_LevelPtr, int directionMoving);
	virtual ~Fireball();

	void Tick(double deltaTime);
	void Paint();

	void SetPaused(bool paused);

private:
	static const double VERTICAL_VEL;
	static const double HORIZONTAL_VEL;

	static const int WIDTH = 16;
	static const int HEIGHT = 16;

	// Our middle fire ball is our inherited actor
	PhysicsActor* m_ActTopBallPtr = nullptr;
	PhysicsActor* m_ActBtmBallPtr = nullptr;

	int m_DirMoving;
};
