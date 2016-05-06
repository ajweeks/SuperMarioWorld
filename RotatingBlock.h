#pragma once

#include "Block.h"

class RotatingBlock : public Block
{
public:
	RotatingBlock(DOUBLE2 topLeft, Level* levelPtr, bool spawnsBeanstalk);

	RotatingBlock(const RotatingBlock&) = delete;
	RotatingBlock& operator=(const RotatingBlock&) = delete;

	void Tick(double deltaTime);
	void Paint();
	void Hit(Level* levelPtr);
	bool IsRotating();

	static const int FRAMES_OF_ROTATION = 55;

private:
	int m_Rotations = -1;
	bool m_IsRotating = false;

	bool m_SpawnsBeanstalk;
	bool m_ShouldSpawnBeanstalk = false;
};