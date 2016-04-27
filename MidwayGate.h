#pragma once

#include "Gate.h"

class MidwayGate : public Gate
{
public:
	MidwayGate(DOUBLE2 topLeft, Level* levelPtr, int barHeight);
	virtual ~MidwayGate();

	void Tick(double deltaTime);
	void Paint();
	void PaintFrontPole();
	void Hit();

	bool IsHit();

private:
	static const int TILES_WIDE = 2;
	static const int TILES_HIGH = 4;
	static const int BAR_DIAMETER = 3;
	static const int POLE_WIDTH = 8;
	static const int BAR_LENGTH = 20;
};
