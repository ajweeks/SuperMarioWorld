#include "stdafx.h"

#include "MoveableItem.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "Block.h"

MoveableItem::MoveableItem(DOUBLE2 topLeft, Type itemType, int spriteSheetXIndex, int spriteSheetYIndex, Level* levelPtr, int directionFacing) :
	Item(topLeft, itemType, levelPtr, Level::ITEM, BodyType::DYNAMIC, WIDTH, HEIGHT), 
	m_SpawnLocation(topLeft),
	m_DirFacing(directionFacing),
	m_SpriteSheetIndex(INT2(spriteSheetXIndex, spriteSheetYIndex))
{
	assert(directionFacing == Direction::LEFT || directionFacing == Direction::RIGHT);

	m_ActPtr->SetSensor(true);
	m_ActPtr->SetGravityScale(0.0);

	b2Filter collisionFilter;
	collisionFilter.categoryBits = Level::ITEM;
	collisionFilter.maskBits = Level::LEVEL | Level::BLOCK | Level::PLAYER | Level::YOSHI_TOUNGE;
	m_ActPtr->SetCollisionFilter(collisionFilter);

	m_IntroAnimationTimer = CountdownTimer(95);
	m_IntroAnimationTimer.Start();
}

void MoveableItem::Tick(double deltaTime)
{
	if (m_IntroAnimationTimer.Tick())
	{
		if (m_IntroAnimationTimer.IsComplete())
		{
			m_ActPtr->SetSensor(false);
			m_ActPtr->SetGravityScale(1.0);
		}
		else if (m_IntroAnimationTimer.FramesElapsed() > FRAMES_OF_ANIMATION_DELAY)
		{
			DOUBLE2 prevPos = m_ActPtr->GetPosition();
			m_ActPtr->SetPosition(prevPos.x, prevPos.y - 0.2);
		}
	}
	else
	{
		DOUBLE2 point1 = m_ActPtr->GetPosition();
		DOUBLE2 point2 = m_ActPtr->GetPosition() + DOUBLE2(m_DirFacing * 7, 0);
		DOUBLE2 intersection, normal;
		double fraction = -1.0;

		// If this returns true, we are about to hit an obstacle, let's turn around
		if (m_LevelPtr->Raycast(point1, point2, m_CollisionBits, intersection, normal, fraction))
		{
			m_DirFacing = -m_DirFacing;
		}

		// Remove items which fall off the left edge of the level
		if (m_ActPtr->GetPosition().x < -WIDTH)
		{
			m_LevelPtr->RemoveItem(this);
		}

		double xVel = HORIZONTAL_VEL * m_DirFacing;
		m_ActPtr->SetLinearVelocity(DOUBLE2(xVel, m_ActPtr->GetLinearVelocity().y));
	}
}

void MoveableItem::Paint()
{
	if (m_IntroAnimationTimer.IsActive() == false || m_IntroAnimationTimer.FramesElapsed() > FRAMES_OF_ANIMATION_DELAY)
	{
		double left = m_ActPtr->GetPosition().x;
		double top = m_ActPtr->GetPosition().y + 2;

		SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES)->Paint(left, top, m_SpriteSheetIndex.x, m_SpriteSheetIndex.y);
	}

	// We want to appear as though we are behind the prize block
	// but this is tough to do since things are painted in the order
	// they are added in. So we'll just paint a 'fake' prize block
	// exactly in the correct position
	if (m_IntroAnimationTimer.IsActive() && m_IntroAnimationTimer.FramesElapsed() > FRAMES_OF_ANIMATION_DELAY)
	{
		double left = m_SpawnLocation.x + WIDTH / 2;
		double top = m_SpawnLocation.y + Block::HEIGHT / 2 + 2;
		SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES)->Paint(left, top, 4, 4);
	}
}

bool MoveableItem::IsAnimating()
{
	return  m_IntroAnimationTimer.IsActive();
}

void MoveableItem::SetPaused(bool paused)
{
	// if we are a static item (at the top of the screen), or
	// we are still animating out of our prize block, we should stay
	// deactivated
	if (m_IsFalingFromTopOfScreen == false && m_IntroAnimationTimer.IsActive() == false)
	{
		m_ActPtr->SetActive(!paused);
	}
}
