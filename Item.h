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
		KOOPA_SHELL, FIREBALL,
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

	void Tick(double deltaTime);
	virtual void Paint() = 0;

	void SetLinearVelocity(DOUBLE2 newVel);
	void SetPosition(DOUBLE2 newPos);

	static Type StringToTYPE(std::string string);
	static std::string TYPEToString(Type type);

	// Similar to StringToTYPE, but this carries extra data with it
	static Item* StringToItem(const std::string& itemString, DOUBLE2 itemPos, Level* levelPtr);
	static std::string ItemToString(Item* itemPtr);

	Type GetType();
	bool IsBlock();

	static const int TILE_SIZE = 16;
protected:
	static const int MINIMUM_PLAYER_DISTANCE; // how close the player needs to get for us to activate

	const int WIDTH;
	const int HEIGHT;
	RECT2 m_Bounds;
	bool m_IsActive;
	DOUBLE2 m_SpawningPosition;

private:
	Type m_Type;
};
