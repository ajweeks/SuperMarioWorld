#include "stdafx.h"

#include "SuperMushroom.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "Block.h"

SuperMushroom::SuperMushroom(DOUBLE2 topLeft, Level* levelPtr, int directionFacing, bool isStatic) :
	Item(topLeft, TYPE::SUPER_MUSHROOM, levelPtr, BodyType::DYNAMIC, WIDTH, HEIGHT), 
	m_IsStatic(isStatic), m_SpawnLocation(topLeft)
{
	assert(directionFacing == 1 || directionFacing == -1);

	m_DirFacing = directionFacing;

	m_ActPtr->SetActive(false);

	if (m_IsStatic == false)
	{
		m_FramesOfIntroAnimation = 0;
	}
}

void SuperMushroom::Tick(double deltaTime)
{
	if (m_IsStatic)
	{
		double xPos = m_LevelPtr->GetCameraOffset().x + Game::WIDTH / 2;
		double yPos = m_LevelPtr->GetCameraOffset().y + 25;
		m_ActPtr->SetPosition(DOUBLE2(xPos, yPos));
	}
	else if(m_FramesOfIntroAnimation > -1)
	{
		++m_FramesOfIntroAnimation;

		if (m_FramesOfIntroAnimation > FRAMES_OF_INTRO_ANIMATION)
		{
			m_FramesOfIntroAnimation = -1;
			m_ActPtr->SetActive(true);
		}
		else if (m_FramesOfIntroAnimation > FRAMES_OF_ANIMATION_DELAY)
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

		// If this returns true, the koopa is about to hit an obstacle, let's turn around
		if (m_LevelPtr->Raycast(point1, point2, m_CollisionBits, intersection, normal, fraction))
		{
			m_DirFacing = FacingDirection::OppositeDirection(m_DirFacing);
		}

		double xVel = HORIZONTAL_VEL * m_DirFacing;
		m_ActPtr->SetLinearVelocity(DOUBLE2(xVel, m_ActPtr->GetLinearVelocity().y));
	}
}

void SuperMushroom::Paint()
{
	if (m_FramesOfIntroAnimation == -1 || m_FramesOfIntroAnimation > FRAMES_OF_ANIMATION_DELAY)
	{
		double left = m_ActPtr->GetPosition().x;
		double top = m_ActPtr->GetPosition().y + 2;
		double srcCol = 2;
		double srcRow = 12;

		SpriteSheetManager::generalTilesPtr->Paint(left, top, srcCol, srcRow);
	}

	// We want to appear as though we are behind the prize block
	// but this is tough to do since things are painted in the order
	// they are added in. So we'll just paint a 'fake' prize block
	// exactly in the correct position
	if (m_FramesOfIntroAnimation > FRAMES_OF_ANIMATION_DELAY)
	{
		double left = m_SpawnLocation.x + WIDTH / 2;
		double top = m_SpawnLocation.y + Block::HEIGHT / 2 + 2;
		SpriteSheetManager::generalTilesPtr->Paint(left, top, 4, 4);
	}
}

bool SuperMushroom::IsAnimating()
{
	return m_FramesOfIntroAnimation > -1;
}

void SuperMushroom::SetPaused(bool paused)
{
	// if we are a static mushroom (at the top of the screen), or
	// we are still animating out of our prize block, we should stay
	// deactivated
	if (m_IsStatic == false && m_FramesOfIntroAnimation == -1)
	{
		m_ActPtr->SetActive(!paused);
	}
}