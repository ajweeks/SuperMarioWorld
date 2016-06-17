#include "stdafx.h"

#include "BlockBreakParticle.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

BlockChunk::BlockChunk(DOUBLE2 position, DOUBLE2 velocity, bool isRainbow) :
	m_Position(position), m_Velocity(velocity), m_IsRanbow(isRainbow)
{
	m_BlockType = rand() % 4;
	m_TypeTimer = rand() % 4 + 1;
	m_AnimInfo.secondsPerFrame = 0.02;
}
BlockChunk::~BlockChunk()
{
}
void BlockChunk::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 6;

	if (m_TypeTimer-- < 0)
	{
		m_BlockType = rand() % 4;
		m_TypeTimer = rand() % 4 + 1;
	}

	m_Position += m_Velocity * deltaTime;
	m_Velocity.y += 12;
}
void BlockChunk::Paint()
{
	double left = m_Position.x;
	double top = m_Position.y;

	SpriteSheet* spriteSheetToPaint;
	if (m_IsRanbow) spriteSheetToPaint = SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES);
	else spriteSheetToPaint = SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::MONTY_MOLE);

	const double tileSize = spriteSheetToPaint->GetTileWidth();
	const double halfTileSize = tileSize / 2.0;

	double srcX = (m_BlockType % 2) * halfTileSize;
	double srcY = int(m_BlockType / 2) * halfTileSize;
	if (m_IsRanbow) 
	{
		srcX += (m_AnimInfo.frameNumber * tileSize);
		srcY += 12 * tileSize;
	}
	else 
	{
		srcX += (6 * tileSize);
	}

	RECT2 srcRect(srcX, srcY, srcX + halfTileSize, srcY + halfTileSize);

	GAME_ENGINE->DrawBitmap(spriteSheetToPaint->GetBitmap(), DOUBLE2(left, top), srcRect);
}



BlockBreakParticle::BlockBreakParticle(DOUBLE2 centerPos, bool isGrabBlock) : 
	Particle(LIFETIME, centerPos)
{
	int offsetX = 2;
	int offsetY = 2;
	m_BlockChunkPtrArr[0] = new BlockChunk(centerPos + DOUBLE2(-offsetX, offsetY), DOUBLE2(-40, -280), isGrabBlock);
	m_BlockChunkPtrArr[1] = new BlockChunk(centerPos + DOUBLE2(-offsetX, 0), DOUBLE2(-40, -160), isGrabBlock);
	m_BlockChunkPtrArr[2] = new BlockChunk(centerPos + DOUBLE2(offsetX, offsetY), DOUBLE2(40, -280), isGrabBlock);
	m_BlockChunkPtrArr[3] = new BlockChunk(centerPos + DOUBLE2(offsetX, 0), DOUBLE2(40, -160), isGrabBlock);
}

BlockBreakParticle::~BlockBreakParticle()
{
	for (int i = 0; i < 4; ++i)
	{
		delete m_BlockChunkPtrArr[i];
	}
}

bool BlockBreakParticle::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	--m_LifeRemaining;

	for (int i = 0; i < 4; ++i)
	{
		m_BlockChunkPtrArr[i]->Tick(deltaTime);
	}

	return (m_LifeRemaining < 0);
}

void BlockBreakParticle::Paint()
{
	for (int i = 0; i < 4; ++i)
	{
		m_BlockChunkPtrArr[i]->Paint();
	}
}
