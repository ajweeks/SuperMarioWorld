#pragma once

#include "Gate.h"

class GoalGate : public Gate
{
public:
	GoalGate(DOUBLE2 topLeft, Level* levelPtr);
	virtual ~GoalGate();

	void Tick(double deltaTime);
	void Paint();
	void PaintFrontPole();
	void Hit();

	bool IsHit();

private:
	static const int TILES_WIDE = 3;
	static const int TILES_HIGH = 9;
	static const int BAR_DIAMETER = 9; // (How many pixels tall the bar is)
	static const int BAR_LENGTH = 24;
	static const int POLE_WIDTH = 16;

	static const double BAR_SPEED;

	int m_BarDirectionMoving; // The bar 'bounces' back and forth from the top to the bottom of the poles
};
