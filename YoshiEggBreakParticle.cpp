#include "stdafx.h"

#include "YoshiEggBreakParticle.h"
#include "Particle.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"

YoshiEggBreakParticle::YoshiEggBreakParticle(DOUBLE2 position) : Particle(LIFETIME, position)
{
	m_AnimInfo.secondsPerFrame = 0.055;

	DOUBLE2 acc = DOUBLE2(0, -20);
	int xO = 6;
	int yO = 8;
	m_ShellPiecesArr[0] = ShellPiece(position + DOUBLE2(-xO, -yO), DOUBLE2(-10, -60), acc); // Top L
	m_ShellPiecesArr[1] = ShellPiece(position + DOUBLE2(xO, -yO), DOUBLE2(10, -60), acc);   // Top R
	m_ShellPiecesArr[2] = ShellPiece(position + DOUBLE2(-xO, yO), DOUBLE2(-10, -50), acc);  // Btm L
	m_ShellPiecesArr[3] = ShellPiece(position + DOUBLE2(xO, yO), DOUBLE2(10, -50), acc);    // Btm R
}

YoshiEggBreakParticle::~YoshiEggBreakParticle()
{
}

bool YoshiEggBreakParticle::Tick(double deltaTime)
{
	m_AnimInfo.Tick(deltaTime);
	m_LifeRemaining = LIFETIME - m_AnimInfo.frameNumber - 1;

	for (size_t i = 0; i < NUM_PIECES; ++i)
	{
		//m_ShellPiecesArr[i].m_Vel += m_ShellPiecesArr[i].m_Acc;
		//m_ShellPiecesArr[i].m_Pos += m_ShellPiecesArr[i].m_Vel;
	}

	return (m_LifeRemaining < 0);
}

void YoshiEggBreakParticle::Paint()
{
	SpriteSheet* eggBreakParticlePtr = SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::YOSHI_EGG_BREAK_PARTICLE);
	
	int col = 0, row = 0;
	if (m_AnimInfo.frameNumber == 1) col += 2;

	eggBreakParticlePtr->Paint(m_ShellPiecesArr[0].m_Pos.x, m_ShellPiecesArr[0].m_Pos.y, col, row);
	eggBreakParticlePtr->Paint(m_ShellPiecesArr[1].m_Pos.x, m_ShellPiecesArr[1].m_Pos.y, col + 1, row);
	eggBreakParticlePtr->Paint(m_ShellPiecesArr[2].m_Pos.x, m_ShellPiecesArr[2].m_Pos.y, col, row + 1);
	eggBreakParticlePtr->Paint(m_ShellPiecesArr[3].m_Pos.x, m_ShellPiecesArr[3].m_Pos.y, col + 1, row + 1);
}
