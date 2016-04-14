#pragma once

#include "Item.h"

class Coin : public Item
{
public:
	Coin(DOUBLE2 topLeft, Level* levelPtr, int life = -1, Item::TYPE type = Item::TYPE::COIN, DOUBLE2 size = DOUBLE2(WIDTH, HEIGHT));

	void Tick(double deltaTime);
	void Paint();
	int GetLifeRemaining();
	void GenerateParticles(); // Called when this coin is collected

	static const int LIFETIME = 25;

protected:
	static const int WIDTH = 16;
	static const int HEIGHT = 16;

private:
	int m_Life;

};