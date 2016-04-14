#pragma once

#include "Block.h"

class RotatingBlock : public Block
{
public:
	RotatingBlock(DOUBLE2 topLeft, Level* levelPtr);
	void Tick(double deltaTime);
	void Paint();
	void Hit(Level* levelPtr);
	bool IsRotating();

	static const int MAX_ROTATIONS = 25;

private:
	int m_Rotations = -1;
	bool m_IsRotating = false;

};