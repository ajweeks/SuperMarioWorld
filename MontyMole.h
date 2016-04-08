#pragma once

#include "Enemy.h"

class MontyMole : public Enemy
{
public:
	MontyMole(DOUBLE2& startingPos, Level* levelPtr);
	virtual ~MontyMole();

	void Tick(double deltaTime);
	void Paint();

	int GetWidth();
	int GetHeight();

private:
	static const int WIDTH = 16;
	static const int HEIGHT = 16;

};

