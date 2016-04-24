#pragma once

#include "Item.h"

class Beanstalk : public Item
{
public:
	Beanstalk(DOUBLE2 bottomLeft, Level* levelPtr, int height);
	virtual ~Beanstalk();
	
	void Tick(double deltaTime);
	void Paint();

private:
	const int FINAL_HEIGHT;
	const int VINE_WIDTH = 6;
	int m_CurrentHeight;
};
