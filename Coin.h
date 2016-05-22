#pragma once

#include "Item.h"

class Coin : public Item
{
public:
	Coin(DOUBLE2 topLeft, Level* levelPtr, int life = -1, Item::Type type = Item::Type::COIN, DOUBLE2 size = DOUBLE2(WIDTH, HEIGHT));

	Coin(const Coin&) = delete;
	Coin& operator=(const Coin&) = delete;

	void Tick(double deltaTime);
	void Paint();
	int GetLifeRemaining();
	void GenerateParticles(); // Called when this coin is collected

	static const int LIFETIME = 25;

	void TurnToBlock(bool toBlock);

	bool IsBlock();

protected:
	static const int WIDTH = 16;
	static const int HEIGHT = 16;

private:
	int m_Life;
	bool m_IsBlock = false; // This is true for a short period of time after mario hits a p-switch

};