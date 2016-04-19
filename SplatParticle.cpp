#include "stdafx.h"

#include "SplatParticle.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "Game.h"

SplatParticle::SplatParticle(DOUBLE2& positionRef) :
	Particle(LIFETIME, positionRef)
{
}

bool SplatParticle::Tick(double deltaTime)
{
	--m_LifeRemaining;

	return false;
}

void SplatParticle::Paint()
{
	MATRIX3X2 matPrevWorld = GAME_ENGINE->GetWorldMatrix();

	// These particles flip horizontally every two frames
	if (m_LifeRemaining % 4 >= 2)
	{
		MATRIX3X2 matTranlate = MATRIX3X2::CreateTranslationMatrix(m_Position.x, m_Position.y);
		MATRIX3X2 matReflect = MATRIX3X2::CreateScalingMatrix(-1, 1);
		GAME_ENGINE->SetWorldMatrix(matTranlate.Inverse() * matReflect * matTranlate);
	}

	GAME_ENGINE->DrawBitmap(SpriteSheetManager::splatParticlePtr, m_Position.x, m_Position.y);

	GAME_ENGINE->SetWorldMatrix(matPrevWorld);
}
