#pragma once

#include "Coin.h"

class DragonCoin : public Coin
{
public:
	DragonCoin(DOUBLE2 topLeft, Level* levelPtr);
	virtual ~DragonCoin();

	DragonCoin(const DragonCoin&) = delete;
	DragonCoin& operator=(const DragonCoin&) = delete;

	void Tick(double deltaTime);
	void Paint();
	void GenerateParticles();
	
	static const int WIDTH = 16;
	static const int HEIGHT = 32;
};
