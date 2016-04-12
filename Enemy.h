#pragma once

#include "Entity.h"
#include "Enumerations.h"

class Enemy : public Entity
{
public:
	enum class TYPE
	{
		NONE, KOOPA_TROOPA, CHARGIN_CHUCK, PIRHANA_PLANT, MONTY_MOLE, // POKEY, BULLET_BILL, DRY_BONES, BOO
	};

	Enemy(TYPE type, DOUBLE2& posRef, double width, double height, SpriteSheet* spriteSheetPtr, BodyType bodyType,
		Level* levelPtr, void* userPointer = nullptr);
	virtual ~Enemy();

	virtual void Tick(double deltaTime) = 0;
	virtual void Paint() = 0;

	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;

	TYPE GetType();

protected:
	TYPE m_Type;

	int m_DirFacing;
	int m_DirFacingLastFrame;

	bool m_IsOnGround;

};
