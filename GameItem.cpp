#pragma once

#include "stdafx.h"

#include "GameItem.h"
#include "Enumerations.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "LevelData.h"
#include "Level.h"


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
Item::Item(DOUBLE2 topLeft, DOUBLE2 bottomRight, TYPE type, BodyType bodyType) :
	Entity(topLeft + DOUBLE2((bottomRight.x - topLeft.x) / 2, (bottomRight.y - topLeft.y) / 2),
		SpriteSheetManager::generalTiles, bodyType, this), m_Type(type)
{
	double width = bottomRight.x - topLeft.x;
	double height = bottomRight.y - topLeft.y;
	m_ActPtr->AddBoxFixture(width, height, 0.0);
	m_ActPtr->SetUserData(int(ActorId::ITEM));
	// TODO: Figure out if all items have a fixed rotation
	//m_ActPtr->SetFixedRotation(true);
}
Item::~Item() {}
void Item::AddContactListener(ContactListener* listener)
{
	m_ActPtr->AddContactListener(listener);
}
Item::TYPE Item::GetType()
{
	return m_Type;
}
void Item::TogglePaused(bool paused)
{
	m_ActPtr->SetActive(!paused);
}


// ___COIN___
// NOTE: There are two types of regular coins, those spawned at the start of the game,
// which have an infinite lifetime, and those which are spawned from prize blocks, which are only visible for 
// a few frames. If m_LifeTicks == -1, this coin is the former, otherwise it is the latter
Coin::Coin(DOUBLE2 topLeft, int life, TYPE type, DOUBLE2 size) :
	Item(topLeft,
		topLeft + size,
		type), m_Life(life)
{
	m_ActPtr->SetFixedRotation(true);
	m_ActPtr->SetSensor(true);

	if (life > -1)
	{
		// This coin shoots up, then falls down, then disapears
		m_ActPtr->SetBodyType(BodyType::DYNAMIC);
		m_ActPtr->SetLinearVelocity(DOUBLE2(0, -330));
	}
}
bool Coin::Tick(double deltaTime, Level* levelPtr)
{
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 4;

	if (m_Life > -1)
	{
		if (++m_Life > LIFETIME)
		{
			m_Life = -1; // Not really necesary... right?
			return true;
		}
	}

	return false;
}
void Coin::Paint()
{
	double srcX = 0 + m_AnimInfo.frameNumber * WIDTH;
	double srcY = 0;
	RECT2 srcRect = RECT2(srcX, srcY, srcX + WIDTH, srcY + HEIGHT);
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	SpriteSheetManager::generalTiles->Paint(left, top, srcRect);
}

// ___DRAGON COIN___
// NOTE: Dragon coins always haev infinite life, therefore -1 as life param
DragonCoin::DragonCoin(DOUBLE2 centerPos) :
	Coin(centerPos, -1, TYPE::DRAGON_COIN, DOUBLE2(WIDTH,HEIGHT))
{
}
bool DragonCoin::Tick(double deltaTime, Level* levelPtr)
{
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 6;

	return false;
}
void DragonCoin::Paint()
{
	double srcX = 0 + m_AnimInfo.frameNumber * WIDTH;
	double srcY = 2 * Coin::HEIGHT;
	RECT2 srcRect = RECT2(srcX, srcY, srcX + WIDTH, srcY + HEIGHT);
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y - 18;
	SpriteSheetManager::generalTiles->Paint(left, top, srcRect);
}


// ___BLOCK___
Block::Block(DOUBLE2 topLeft, TYPE type) :
	Item(topLeft, topLeft + DOUBLE2(WIDTH, HEIGHT), type)
{
}


// ___PRIZE BLOCK___
PrizeBlock::PrizeBlock(DOUBLE2 topLeft) :
	Block(topLeft, TYPE::PRIZE_BLOCK)
{
}
bool PrizeBlock::Tick(double deltaTime, Level* levelPtr)
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
	return false;
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
DOUBLE2 PrizeBlock::Hit()
{
	if (m_IsUsed == false)
	{
		m_IsUsed = true;
		m_CurrentFrameOfBumpAnimation = 0;
		m_yo = 0;

		return DOUBLE2(m_ActPtr->GetPosition().x - WIDTH/2, m_ActPtr->GetPosition().y - HEIGHT);
	}

	// TODO: return a better null value?
	return DOUBLE2();
}

// ___EXCLAMATION MARK BLOCK___
ExclamationMarkBlock::ExclamationMarkBlock(DOUBLE2 topLeft, COLOUR colour, bool isSolid) :
	Block(topLeft, TYPE::EXCLAMATION_MARK_BLOCK), m_Colour(colour)
{
	m_AnimInfo.frameNumber = 0;
	SetSolid(isSolid);
}
bool ExclamationMarkBlock::Tick(double deltaTime, Level* levelPtr)
{
	return false;
}
void ExclamationMarkBlock::Paint()
{
	double srcX = 1 * WIDTH;
	double srcY = 10 * HEIGHT;
	if (m_IsSolid == false)
	{
		srcY -= HEIGHT;
	}
	
	RECT2 srcRect = RECT2(srcX, srcY, srcX + WIDTH, srcY + HEIGHT);
	double left = m_ActPtr->GetPosition().x;
	double top = m_ActPtr->GetPosition().y;
	SpriteSheetManager::generalTiles->Paint(left, top, srcRect);
}
void ExclamationMarkBlock::SetSolid(bool solid)
{
	m_IsSolid = solid;
	m_ActPtr->SetSensor(!solid);
}


// ___ROTATING BLOCK___
RotatingBlock::RotatingBlock(DOUBLE2 topLeft) :
	Block(topLeft, TYPE::ROTATING_BLOCK)
{
}
bool RotatingBlock::Tick(double deltaTime, Level* levelPtr)
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
	return false;
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
	Block(topLeft, TYPE::MESSAGE_BLOCK), m_Message(message)
{
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