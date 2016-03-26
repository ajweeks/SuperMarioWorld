#pragma once

class SpriteSheet
{
public:
	SpriteSheet(const String filePath, int tilesWide, int tilesHigh, int tileWidth, int tileHeight);
	virtual ~SpriteSheet();

	SpriteSheet(const SpriteSheet&) = delete;
	SpriteSheet& operator=(const SpriteSheet&) = delete;

	void SetTransparencyColor(COLOR& colorRef);
	void Paint(double centerX, double centerY, RECT2 srcRect);

	int GetTileWidth();
	int GetTileHeight();

private:
	Bitmap *m_BmpSpriteSheetPtr;

	int m_TilesWide;
	int m_TilesHigh;
	int m_TileWidth;
	int m_TileHeight;
};
