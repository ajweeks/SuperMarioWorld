#include "stdafx.h"

#include "StarParticle.h"
#include "Game.h"
#include "SpriteSheetManager.h"	
#include "SpriteSheet.h"

StarParticle::StarParticle(DOUBLE2& positionRef, DOUBLE2& velocityRef) :
	Particle(LIFETIME, positionRef), 
	m_Velocity(velocityRef)
{
}

bool StarParticle::Tick(double deltaTime)
{
	m_Position += m_Velocity * deltaTime;

	return false;
}

void StarParticle::Paint()
{
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::starParticlePtr, m_Position.x, m_Position.y);
}