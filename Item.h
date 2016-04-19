#pragma once

#include "Entity.h"

class Item : public Entity
{
public:
	enum class TYPE
	{
		NONE,
		COIN, DRAGON_COIN, BERRY, KEY, KEYHOLE, P_SWITCH,
		ONE_UP_MUSHROOM, THREE_UP_MOON,
		KOOPA_SHELL,
		SUPER_MUSHROOM, FIRE_FLOWER, CAPE_FEATHER, STAR, POWER_BALLOON,
		PRIZE_BLOCK, MESSAGE_BLOCK, ROTATING_BLOCK, EXCLAMATION_MARK_BLOCK
	};

	Item(DOUBLE2 topLeft, TYPE type, Level* levelPtr,
		BodyType bodyType = BodyType::STATIC, int width = 16, int height = 16);
	virtual ~Item();

	virtual void Tick(double deltaTime) = 0;
	virtual void Paint() = 0;

	static TYPE StringToTYPE(std::string string);
	static std::string TYPEToString(TYPE type);

	TYPE GetType();
	bool IsBlock();

protected:
	const int WIDTH;
	const int HEIGHT;
	RECT2 m_Bounds;

private:
	TYPE m_Type;

	// TODO: Add velocity member? (And an isStatic field? or just a vel of 0)
};