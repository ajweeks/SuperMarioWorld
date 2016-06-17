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
	void Hit();
	bool IsRotating();

private:
	SMWTimer m_RotationTimer;

	bool m_SpawnsBeanstalk;
	bool m_ShouldSpawnBeanstalk = false; // Set to true when the player hits this block

	SMWTimer m_BumpAnimationTimer;
	int m_yo = 0;
	bool m_IsUsed = false;
};
