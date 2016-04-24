#pragma once
#include "Item.h"
class MidwayGate : public Item
{
public:
	MidwayGate(DOUBLE2 topLeft, Level* levelPtr, int barHeight);
	virtual ~MidwayGate();

	void Tick(double deltaTime);
	void Paint();
	void Hit();

	bool IsHit();

private:
	static const int TILES_WIDE = 2;
	static const int TILES_HIGH = 4;
	static const int BAR_HEGHT = 3;
	static const int POLE_WIDTH = 8;

	DOUBLE2 m_TopLeft;
	bool m_IsHit = false;
	int m_BarHeight;
};
