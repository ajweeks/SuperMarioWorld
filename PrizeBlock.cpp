#include "stdafx.h"

#include "PrizeBlock.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "SoundManager.h"
#include "Level.h"
#include "Coin.h"
#include "Yoshi.h"
#include "OneUpMushroom.h"
#include "SuperMushroom.h"
#include "Game.h"

PrizeBlock::PrizeBlock(DOUBLE2 topLeft, Level* levelPtr, std::string spawnTypeStr, bool isFlyer) :
	Block(topLeft, Type::PRIZE_BLOCK, levelPtr),
	m_SpawnTypeStr(spawnTypeStr),
	m_IsFlyer(isFlyer)
{
	m_AnimInfo.secondsPerFrame = 0.09;
	m_BumpAnimationTimer = SMWTimer(14);
	
	if (isFlyer)
	{
		m_AnimInfo.secondsPerFrame = 0.12;
		m_IsFlying = true;
		m_ActPtr->SetBodyType(BodyType::DYNAMIC);
		m_ActPtr->SetGravityScale(0.0);
	}
}

void PrizeBlock::Tick(double deltaTime)
{
	if (m_ShouldSpawnItem)
	{
		m_ShouldSpawnItem = false;

		DOUBLE2 itemPos(m_ActPtr->GetPosition().x - WIDTH / 2, m_ActPtr->GetPosition().y - HEIGHT / 2);
		if (!m_SpawnTypeStr.compare("Yoshi"))
		{
			if (m_LevelPtr->IsYoshiAlive())
			{
				OneUpMushroom* oneUpMushroomPtr = new OneUpMushroom(itemPos + DOUBLE2(3, -2), m_LevelPtr);
				m_LevelPtr->AddItem(oneUpMushroomPtr, true);
			}
			else
			{
				Yoshi* newYoshiPtr = new Yoshi(itemPos + DOUBLE2(9, -HEIGHT/2), m_LevelPtr);
				m_LevelPtr->AddYoshi(newYoshiPtr);
			}
		}
		else
		{
			Coin* newCoinPtr = new Coin(itemPos, m_LevelPtr, Coin::LIFETIME);
			m_LevelPtr->AddItem(newCoinPtr);
		}
	}

	if (m_BumpAnimationTimer.Tick())
	{
		// NOTE: I *think* what happens during the bump animation is
		//		 the block renders the normal question mark (not spinning)
		//       and moves up then down around 3 or 5 pixels.
		//		 Then the block is "used" and renders the brown used block texture

		m_yo = m_BumpAnimationTimer.FramesElapsed();
		if (m_yo > m_BumpAnimationTimer.TotalFrames() / 2)
		{
			m_yo = (m_BumpAnimationTimer.TotalFrames() / 2) - (m_yo - (m_BumpAnimationTimer.TotalFrames() / 2));
		}
		m_yo = int(m_yo * -0.5);

		if (m_BumpAnimationTimer.IsComplete())
		{
			m_yo = 0;
		}
	}
	else
	{
		m_AnimInfo.Tick(deltaTime);
		if (m_IsFlyer && m_IsFlying) m_AnimInfo.frameNumber %= 2;
		else m_AnimInfo.frameNumber %= 4;
	}

	if (m_IsFlyer)
	{
		if (m_ShouldFreeze)
		{
			m_ShouldFreeze = false;
			m_ActPtr->SetBodyType(BodyType::STATIC);
			m_IsFlying = false;
		}

		if (m_IsFlying)
		{
			m_LifeElapsed += deltaTime;
			double xDist = 45;
			double yDist = 14;
			double xSpeed = 1.25;
			double ySpeed = 3.6;
			double dx = sin(m_LifeElapsed * xSpeed) * xDist;
			double dy = sin(m_LifeElapsed * ySpeed) * yDist;
			DOUBLE2 newPos = m_SpawningPosition + DOUBLE2(dx, dy);
			m_ActPtr->SetPosition(newPos);
		}
	}
}

void PrizeBlock::Paint()
{
	int srcCol = 0 + m_AnimInfo.frameNumber;
	int srcRow = 4;

	int left = (int)m_ActPtr->GetPosition().x;
	int top = (int)m_ActPtr->GetPosition().y + m_yo * 3;

	if (m_IsFlyer && m_IsFlying)
	{
		// Wings
		srcRow = 5;
		srcCol = 4 + m_AnimInfo.frameNumber;
		int wingLeft = left - 7;
		int wingTop = top - 9;
		SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES)->Paint(wingLeft, wingTop, srcCol, srcRow);

		MATRIX3X2 matWorldPrev = GAME_ENGINE->GetWorldMatrix();
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(left + Item::TILE_SIZE / 2, top);
		MATRIX3X2 matReflect = MATRIX3X2::CreateScalingMatrix(-1, 1);
		GAME_ENGINE->SetWorldMatrix(matTranslate.Inverse() * matReflect * matTranslate * matWorldPrev);

		wingLeft += Item::TILE_SIZE;
		SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES)->Paint(wingLeft, wingTop, srcCol, srcRow);

		GAME_ENGINE->SetWorldMatrix(matWorldPrev);

		// Block
		srcRow = 4;
		srcCol = 4;
		SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES)->Paint(left, top, srcCol, srcRow);
	}

	if (m_BumpAnimationTimer.IsActive() && m_IsFlyer == false)
	{
		srcCol = 4;
	}
	else if (m_IsUsed)
	{
		srcCol = 5;
	}

	SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES)->Paint(left, top, srcCol, srcRow);
}

void PrizeBlock::Hit()
{
	SoundManager::PlaySoundEffect(SoundManager::Sound::BLOCK_HIT);

	if (m_IsUsed == false)
	{
		m_IsUsed = true;
		m_BumpAnimationTimer.Start();
		m_yo = 0;

		m_ShouldSpawnItem = true;

		if (m_IsFlyer)
		{
			m_IsFlying = false;
			m_ShouldFreeze = true;
		}
	}
}

void PrizeBlock::SetUsed(bool used)
{
	m_IsUsed = used;
}

bool PrizeBlock::IsFlying() const
{
	return m_IsFlyer && m_IsFlying;
}
