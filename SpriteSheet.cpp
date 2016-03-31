#include "stdafx.h"
#include "SpriteSheet.h"

#define GAME_ENGINE (GameEngine::GetSingleton())

SpriteSheet::SpriteSheet(const String filePath, int tilesWide, int tilesHigh, int tileWidth, int tileHeight) :
	m_TilesWide(tilesWide), m_TilesHigh(tilesHigh), m_TileWidth(tileWidth), m_TileHeight(tileHeight)
{
	m_BmpSpriteSheetPtr = new Bitmap(filePath);
}

SpriteSheet::~SpriteSheet()
{
	delete m_BmpSpriteSheetPtr;
}

void SpriteSheet::Paint(double centerX, double centerY, double col, double row)
{
	assert(col >= 0 && col < m_TilesWide && row >= 0 && row < m_TilesHigh);

	RECT2 srcRect(col * m_TileWidth, row * m_TileHeight, col * m_TileWidth + m_TileWidth, row * m_TileHeight + m_TileHeight);
	GAME_ENGINE->DrawBitmap(m_BmpSpriteSheetPtr, centerX - m_TileWidth / 2, centerY - m_TileHeight / 2, srcRect);
}

void SpriteSheet::SetTransparencyColor(COLOR& colorRef)
{
	m_BmpSpriteSheetPtr->SetTransparencyColor(colorRef);
}

int SpriteSheet::GetTileWidth()
{
	return m_TileWidth;
}

int SpriteSheet::GetTileHeight()
{
	return m_TileHeight;
}