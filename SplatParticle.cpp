#include "stdafx.h"

#include "SplatParticle.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "Game.h"

SplatParticle::SplatParticle(DOUBLE2 position) :
	Particle(LIFETIME, position)
{
}

bool SplatParticle::Tick(double deltaTime)
{
	--m_LifeRemaining;

	return (m_LifeRemaining < 0);
}

void SplatParticle::Paint()
{
	MATRIX3X2 matPrevWorld = GAME_ENGINE->GetWorldMatrix();

	double centerX = m_Position.x + WIDTH / 2.0;
	double centerY = m_Position.y + HEIGHT / 2.0;

	// These particles flip horizontally every two frames
	if (m_LifeRemaining % 4 >= 2)
	{
		MATRIX3X2 matReflect = MATRIX3X2::CreateScalingMatrix(-1, 1);
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(centerX, centerY);
		MATRIX3X2 matTranslateInverse = MATRIX3X2::CreateTranslationMatrix(-centerX, -centerY);
		GAME_ENGINE->SetWorldMatrix(matTranslateInverse * matReflect * matTranslate);
	}

	GAME_ENGINE->DrawBitmap(SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::SPLAT_PARTICLE), m_Position.x, m_Position.y);

	GAME_ENGINE->SetWorldMatrix(matPrevWorld);
}
