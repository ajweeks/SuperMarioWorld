#pragma once

#include "Item.h"
#include "Level.h"

class SuperMushroom : public Item
{
public:
	// horizontalDir is either 1 or -1, signifying which direction to move in (1:right, -1:left)
	SuperMushroom(DOUBLE2 topLeft, Level* levelPtr, int directionFacing = 1, bool isStatic = false);
	void Tick(double deltaTime);
	void Paint();
private:
	static const int HORIZONTAL_VEL = 80;
	const int m_CollisionBits = Level::COLLIDE_WITH_LEVEL | Level::COLLIDE_WITH_PLAYER;

	int m_DirFacing;
	bool m_IsStatic;

};