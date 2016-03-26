#pragma once

#include "stdafx.h"

#include "GameItem.h"
#include "Enumerations.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "LevelData.h"


// ___PLATFORM___
Platform::Platform(DOUBLE2 topLeft, DOUBLE2 bottomRight)
{
	m_Bounds = RECT2(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
	double width = bottomRight.x - topLeft.x;
	double height = bottomRight.y - topLeft.y;
	m_ActPtr = new PhysicsActor(topLeft + DOUBLE2(width / 2, height / 2), 0, BodyType::STATIC);
	m_ActPtr->AddBoxFixture(width, height, 0.0);
	m_ActPtr->SetUserData(int(ActorId::PLATFORM));
}
Platform::~Platform()
{
	delete m_ActPtr;
}
void Platform::AddContactListener(ContactListener* listener)
{
	m_ActPtr->AddContactListener(listener);
}


// ___PIPE___
Pipe::Pipe(DOUBLE2 topLeft, DOUBLE2 bottomRight, Colour colour, bool canAccess) :
	m_Colour(colour), m_CanAccess(canAccess)
{
	m_Bounds = RECT2(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
	double width = bottomRight.x - topLeft.x;
	double height = bottomRight.y - topLeft.y;
	m_ActPtr = new PhysicsActor(topLeft + DOUBLE2(width / 2, height / 2), 0, BodyType::STATIC);
	m_ActPtr->AddBoxFixture(width, height, 0.0);
	m_ActPtr->SetUserData(int(ActorId::PIPE));
}
Pipe::~Pipe()
{
	delete m_ActPtr;
}
void Pipe::AddContactListener(ContactListener* listener)
{
	m_ActPtr->AddContactListener(listener);
}


// ___ITEM___
Item::Item(DOUBLE2 topLeft, DOUBLE2 bottomRight) :
	Entity(topLeft + DOUBLE2((bottomRight.x - topLeft.x) / 2, (bottomRight.y - topLeft.y) / 2),
		SpriteSheetManager::generalTiles,
		BodyType::KINEMATIC,
		this)
{
	double width = bottomRight.x - topLeft.x;
	double height = bottomRight.y - topLeft.y;
	m_ActPtr->AddBoxFixture(width, height, 0.0);
	m_ActPtr->SetUserData(int(ActorId::ITEM));
}
Item::~Item() {}
void Item::AddContactListener(ContactListener* listener)
{
	m_ActPtr->AddContactListener(listener);
}
/*void SetUserPointer(void* userPointer)
{
if (typeid(userPointer) != typeid(Item*))
{
GAME_ENGINE->MessageBox(String("INVALID USER POINTER PASSED TO SetUserPointer (LevelData.h)"));
return;
}
m_ActPtr->SetUserPointer(userPointer);
}*/


// ___COIN___
Coin::Coin(DOUBLE2 centerPos) :
	Item(centerPos, centerPos + DOUBLE2(WIDTH, HEIGHT))
{
	m_Type = TYPE::COIN;
}
void Coin::Tick(double deltaTime, Level* levelPtr)
{
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 4;
}
void Coin::Paint()
{
	double srcX = 0 + m_AnimInfo.frameNumber * WIDTH;
	double srcY = 0;
	RECT2 srcRect = RECT2(srcX, srcY, srcX + WIDTH, srcY + HEIGHT);
	double left = m_ActPtr->GetPosition().x - WIDTH / 2;
	double top = m_ActPtr->GetPosition().y - HEIGHT / 2;
	SpriteSheetManager::generalTiles->Paint(left, top, srcRect);
}


// ___DRAGON COIN___
DragonCoin::DragonCoin(DOUBLE2 centerPos) :
	Coin(centerPos)
{
	m_Type = TYPE::DRAGON_COIN;
}
void DragonCoin::Tick(double deltaTime, Level* levelPtr)
{
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 6;
}
void DragonCoin::Paint()
{
	double srcX = 0 + m_AnimInfo.frameNumber * WIDTH;
	double srcY = 2 * HEIGHT;
	RECT2 srcRect = RECT2(srcX, srcY, srcX + WIDTH, srcY + HEIGHT * 2);
	double left = m_ActPtr->GetPosition().x - WIDTH / 2;
	double top = m_ActPtr->GetPosition().y - HEIGHT / 2;
	SpriteSheetManager::generalTiles->Paint(left, top, srcRect);
}


// ___BLOCK___
Block::Block(DOUBLE2 topLeft) :
	Item(topLeft, topLeft + DOUBLE2(WIDTH, HEIGHT))
{
}


// ___PRIZE BLOCK___
PrizeBlock::PrizeBlock(DOUBLE2 topLeft) :
	Block(topLeft)
{
	m_Type = TYPE::PRIZE_BLOCK;
}
void PrizeBlock::Tick(double deltaTime, Level* levelPtr)
{
	if (m_CurrentFrameOfBumpAnimation > -1)
	{
		// NOTE: I *think* what happens during the bump animation is
		//		 the block renders the normal question mark (not spinning)
		//       and moves up then down around 3 or 5 pixels.
		//		 Then the block is "used" and renders the brown used block texture

		m_yo = m_CurrentFrameOfBumpAnimation;
		if (m_yo > m_FramesOfBumpAnimation / 2)
		{
			m_yo = (m_FramesOfBumpAnimation / 2) - (m_yo - (m_FramesOfBumpAnimation / 2));
		}
		m_yo *= -1;

		m_CurrentFrameOfBumpAnimation++;
		if (m_CurrentFrameOfBumpAnimation > m_FramesOfBumpAnimation)
		{
			m_CurrentFrameOfBumpAnimation = -1;
			m_yo = 0;
		}
	}
	else
	{
		m_AnimInfo.Tick(deltaTime);
		m_AnimInfo.frameNumber %= 4;
	}
}
void PrizeBlock::Paint()
{
	double srcX = 0 + m_AnimInfo.frameNumber * WIDTH;
	double srcY = 4 * HEIGHT;

	if (m_IsUsed)
	{
		srcX = 4 * WIDTH;
	}

	RECT2 srcRect = RECT2(srcX, srcY, srcX + WIDTH, srcY + HEIGHT);
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y + m_yo * 3;
	SpriteSheetManager::generalTiles->Paint(left, top, srcRect);
}
void PrizeBlock::Hit(LevelData* levelDataPtr)
{
	if (m_IsUsed == false)
	{
		m_IsUsed = true;
		m_CurrentFrameOfBumpAnimation = 0;
		m_yo = 0;

		// Shoot out coin
		levelDataPtr->AddItem(new Coin(DOUBLE2(m_ActPtr->GetPosition().x + WIDTH / 2, m_ActPtr->GetPosition().y - 10)));
	}
}


// ___EXCLAMATION MARK BLOCK___
ExclamationMarkBlock::ExclamationMarkBlock(DOUBLE2 topLeft, COLOUR colour) :
	Block(topLeft), m_Colour(colour)
{
	m_Type = TYPE::EXCLAMATION_MARK_BLOCK;
	m_AnimInfo.frameNumber = 0;
}
void ExclamationMarkBlock::Tick(double deltaTime, Level* levelPtr)
{
}
void ExclamationMarkBlock::Paint()
{
	double srcX = 1 * WIDTH;
	double srcY = 10 * HEIGHT;
	RECT2 srcRect = RECT2(srcX, srcY, srcX + WIDTH, srcY + HEIGHT);
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	SpriteSheetManager::generalTiles->Paint(left, top, srcRect);
}


// ___ROTATING BLOCK___
RotatingBlock::RotatingBlock(DOUBLE2 topLeft) :
	Block(topLeft)
{
	m_Type = TYPE::ROTATING_BLOCK;
}
void RotatingBlock::Tick(double deltaTime, Level* levelPtr)
{
	if (m_IsRotating)
	{
		m_AnimInfo.Tick(deltaTime);
		m_AnimInfo.frameNumber %= 4;
		if (m_AnimInfo.frameNumber == 3)
		{
			m_Rotations++;
			if (m_Rotations > MAX_ROTATIONS)
			{
				m_Rotations = 0;
				m_IsRotating = false;
			}
		}
	}
}
void RotatingBlock::Paint()
{
	double srcX = 0 + m_AnimInfo.frameNumber * WIDTH;
	double srcY = 5 * HEIGHT;
	RECT2 srcRect = RECT2(srcX, srcY, srcX + WIDTH, srcY + HEIGHT);
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	SpriteSheetManager::generalTiles->Paint(left, top, srcRect);
}


// ___MESSAGE BLOCK___
MessageBlock::MessageBlock(DOUBLE2 topLeft, String message) :
	Block(topLeft), m_Message(message)
{
	m_Type = TYPE::MESSAGE_BLOCK;
	m_AnimInfo.frameNumber = 0;
}
void MessageBlock::Paint()
{
	double srcX = 5 * WIDTH;
	double srcY = 9 * HEIGHT;
	RECT2 srcRect = RECT2(srcX, srcY, srcX + WIDTH, srcY + HEIGHT);
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	SpriteSheetManager::generalTiles->Paint(left, top, srcRect);
}