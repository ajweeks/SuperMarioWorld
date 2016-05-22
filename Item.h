#pragma once

#include "Entity.h"
#include "Level.h"

class Item : public Entity
{
public:
	enum class Type
	{
		NONE,
		COIN, DRAGON_COIN, BERRY, KEY, KEYHOLE, P_SWITCH,
		ONE_UP_MUSHROOM, THREE_UP_MOON,
		KOOPA_SHELL,
		BEANSTALK, CLOUD_BLOCK,
		SUPER_MUSHROOM, FIRE_FLOWER, CAPE_FEATHER, STAR, POWER_BALLOON,
		PRIZE_BLOCK, MESSAGE_BLOCK, ROTATING_BLOCK, EXCLAMATION_MARK_BLOCK, GRAB_BLOCK,
		MIDWAY_GATE, GOAL_GATE
	};

	Item(DOUBLE2 topLeft, Type type, Level* levelPtr,
		int filterCategoryBits = Level::ITEM,
		BodyType bodyType = BodyType::STATIC, int width = TILE_SIZE, int height = TILE_SIZE);
	virtual ~Item();

	Item(const Item&) = delete;
	Item& operator=(const Item&) = delete;

	virtual void Tick(double deltaTime) = 0;
	virtual void Paint() = 0;

	void SetLinearVelocity(DOUBLE2 newVel);
	void SetPosition(DOUBLE2 newPos);

	static Type StringToTYPE(std::string string);
	static std::string TYPEToString(Type type);

	Type GetType();
	bool IsBlock();

	static const int TILE_SIZE = 16;
protected:

	const int WIDTH;
	const int HEIGHT;
	RECT2 m_Bounds;

private:
	Type m_Type;
};
