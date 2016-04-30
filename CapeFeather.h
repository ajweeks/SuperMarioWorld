#pragma once

#include "Item.h"

class CapeFeather : public Item
{
public:
	CapeFeather(DOUBLE2 topLeft, Level* levelPtr);

	CapeFeather(const CapeFeather&) = delete;
	CapeFeather& operator=(const CapeFeather&) = delete;

	void Tick(double deltaTime);
	void Paint();

};