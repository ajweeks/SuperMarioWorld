#pragma once

#include "Block.h"

class CloudBlock : public Block
{
public:
	CloudBlock(DOUBLE2 topLeft, Level* levelPtr);
	virtual ~CloudBlock();

	void Tick(double deltaTime);
	void Paint();
	void Hit(Level* levelPtr);

};
