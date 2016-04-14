#include "stdafx.h"

#include "SuperMushroom.h"
#include "Game.h"
#include "SpriteSheet.h"

SuperMushroom::SuperMushroom(DOUBLE2 topLeft, Level* levelPtr, int directionFacing, bool isStatic) :
	Item(topLeft, TYPE::SUPER_MUSHROOM, levelPtr, BodyType::DYNAMIC, 10, 16), m_IsStatic(isStatic)
{
	assert(directionFacing == 1 || directionFacing == -1);

	m_DirFacing = directionFacing;

	if (m_IsStatic)
	{
		m_ActPtr->SetActive(false);
	}
	else
	{
		m_ActPtr->SetLinearVelocity(DOUBLE2(m_DirFacing * HORIZONTAL_VEL, m_ActPtr->GetLinearVelocity().y));
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

		double xVel = HORIZONTAL_VEL;
		if (m_DirFacing == FacingDirection::LEFT)
		{
			xVel = -xVel;
		}

		m_ActPtr->SetLinearVelocity(DOUBLE2(xVel, m_ActPtr->GetLinearVelocity().y));
	}
}

void SuperMushroom::Paint()
{
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y + 2;
	double srcCol = 2;
	double srcRow = 12;

	m_SpriteSheetPtr->Paint(left, top, srcCol, srcRow);
}
