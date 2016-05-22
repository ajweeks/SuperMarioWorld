#pragma once

#include "Item.h"
#include "Level.h"
#include "INT2.h"

// This class is the base class for all items which can be spawned out of a prize block (excluding coins)
class MoveableItem : public Item
{
public:
	MoveableItem(DOUBLE2 topLeft, Type itemType, int spriteSheetXIndex, int spriteSheetYIndex, Level* levelPtr, int directionFacing = Direction::RIGHT);

	MoveableItem(const MoveableItem&) = delete;
	MoveableItem& operator=(const MoveableItem&) = delete;

	void Tick(double deltaTime);
	void Paint();

	static const int WIDTH = 10;
	static const int HEIGHT = 16;

	void SetPaused(bool paused);

	bool IsAnimating();

private:
	static const int HORIZONTAL_VEL = 80;
	const int m_CollisionBits = Level::LEVEL | Level::BLOCK;

	INT2 m_SpriteSheetIndex;

	DOUBLE2 m_SpawnLocation;
	int m_DirFacing;
	bool m_IsFalingFromTopOfScreen = false;

	static const int FRAMES_OF_ANIMATION_DELAY = 14; // How long to wait before starting the animation
	CountdownTimer m_IntroAnimationTimer;

};
