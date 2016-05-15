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
		BEANSTALK, CLOUD_BLOCK,
		SUPER_MUSHROOM, FIRE_FLOWER, CAPE_FEATHER, STAR, POWER_BALLOON,
		PRIZE_BLOCK, MESSAGE_BLOCK, ROTATING_BLOCK, EXCLAMATION_MARK_BLOCK,
		MIDWAY_GATE, GOAL_GATE
	};

	Item(DOUBLE2 topLeft, TYPE type, Level* levelPtr,
		BodyType bodyType = BodyType::STATIC, int width = TILE_SIZE, int height = TILE_SIZE);
	virtual ~Item();

	Item(const Item&) = delete;
	Item& operator=(const Item&) = delete;

	virtual void Tick(double deltaTime) = 0;
	virtual void Paint() = 0;

	void SetLinearVelocity(DOUBLE2 newVel);
	void SetPosition(DOUBLE2 newPos);

	static TYPE StringToTYPE(std::string string);
	static std::string TYPEToString(TYPE type);

	TYPE GetType();
	bool IsBlock();

	static const int TILE_SIZE = 16;
protected:

	const int WIDTH;
	const int HEIGHT;
	RECT2 m_Bounds;

private:
	TYPE m_Type;
};
