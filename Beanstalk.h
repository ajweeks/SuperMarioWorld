#pragma once

#include "Item.h"

class Beanstalk : public Item
{
public:
	Beanstalk(DOUBLE2 bottomLeft, Level* levelPtr, int height);
	virtual ~Beanstalk();

	Beanstalk(const Beanstalk&) = delete;
	Beanstalk& operator=(const Beanstalk&) = delete;
	
	void Tick(double deltaTime);
	void Paint();

private:
	const int FINAL_HEIGHT;
	const int VINE_WIDTH = 6;
	int m_CurrentHeight;
};
