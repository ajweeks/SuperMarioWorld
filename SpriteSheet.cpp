#include "stdafx.h"

#include "SpriteSheet.h"
#include "Game.h"

SpriteSheet::SpriteSheet(const String filePath, int tilesWide, int tilesHigh) :
	m_TilesWide(tilesWide), m_TilesHigh(tilesHigh)
{
	m_BmpSpriteSheetPtr = new Bitmap(filePath);
	m_TileWidth = m_BmpSpriteSheetPtr->GetWidth() / tilesWide;
	m_TileHeight = m_BmpSpriteSheetPtr->GetHeight() / tilesHigh;
}

SpriteSheet::~SpriteSheet()
{
	delete m_BmpSpriteSheetPtr;
}

void SpriteSheet::Paint(double centerX, double centerY, int col, int row)
{
	assert(col >= 0 && col < m_TilesWide && row >= 0 && row < m_TilesHigh);

	RECT2 srcRect(col * m_TileWidth, row * m_TileHeight, col * m_TileWidth + m_TileWidth, row * m_TileHeight + m_TileHeight);
	GAME_ENGINE->DrawBitmap(m_BmpSpriteSheetPtr, centerX - m_TileWidth / 2, centerY - m_TileHeight / 2, srcRect);
}

Bitmap* SpriteSheet::GetBitmap() const
{
	return m_BmpSpriteSheetPtr;
}
int SpriteSheet::GetTileWidth() const
{
	return m_TileWidth;
}

int SpriteSheet::GetTileHeight() const
{
	return m_TileHeight;
}