#pragma once

#include "Enemy.h"

class PiranhaPlant : public Enemy
{
public:
	PiranhaPlant(DOUBLE2 topLeft, Level* levelPtr);
	virtual ~PiranhaPlant();

	PiranhaPlant(const PiranhaPlant&) = delete;
	PiranhaPlant& operator=(const PiranhaPlant&) = delete;

	void Tick(double deltaTime);
	void Paint();

	int GetWidth();
	int GetHeight();

private:
	bool IsPlayerNearby();
		
	static const int WIDTH = 15;
	static const int HEIGHT = 15;

	static const int MAX_TILE_HEIGHT = 7; // How many tiles this plant can move upward before falling back down
	static const double VERTICAL_VEL_FAST;
	static const double VERTICAL_VEL_SLOW;

	static const int SLOW_DOWN_HEIGHT = MAX_TILE_HEIGHT - 3;

	DOUBLE2 m_StartingPostion;

	SMWTimer m_PausedAtTopTimer;

	double m_VerticalVel;

	// This is true when this piranha plant is at the bottom
	// of its pipe and can't shoot back up since the player is nearby
	bool m_PausedAtBottom = false; 

	// The plant moves up quickly (VERTICAL_VEL_FAST) then slows down near the apex
	// and moves slowly until reaching the pipe again
	int m_DirMoving;

};
