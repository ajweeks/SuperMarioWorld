#pragma once
#include "Enemy.h"
class KoopaTroopa :
	public Enemy
{
public:
	KoopaTroopa(DOUBLE2& startingPos, Level* levelPtr);
	virtual ~KoopaTroopa();

	void Tick(double deltaTime);
	void Paint();

	int GetWidth();
	int GetHeight();

private:
	static const int WIDTH = 16;
	static const int HEIGHT = 26;
};

