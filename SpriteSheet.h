#pragma once

class SpriteSheet
{
public:
	SpriteSheet(const String filePath, int tilesWide, int tilesHigh);
	virtual ~SpriteSheet();

	SpriteSheet(const SpriteSheet&) = delete;
	SpriteSheet& operator=(const SpriteSheet&) = delete;

	void Paint(double centerX, double centerY, int col, int row);

	int GetTileWidth() const;
	int GetTileHeight() const;
	Bitmap* GetBitmap() const;

private:
	Bitmap *m_BmpSpriteSheetPtr;

	int m_TilesWide;
	int m_TilesHigh;
	int m_TileWidth;
	int m_TileHeight;
};
