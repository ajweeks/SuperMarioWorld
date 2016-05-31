#include "stdafx.h"

#include "Item.h"
#include "SpriteSheetManager.h"
#include "Block.h"
#include "Level.h"
#include "Game.h"
#include "Player.h"

const int Item::MINIMUM_PLAYER_DISTANCE = int(Game::WIDTH * (4.0 / 5.0));

Item::Item(DOUBLE2 topLeft, Type type, Level* levelPtr, int filterCategoryBits, BodyType bodyType, int width, int height) :
	Entity(topLeft + DOUBLE2(width / 2, height / 2), bodyType, levelPtr, ActorId::ITEM, this),
	m_Type(type), WIDTH(width), HEIGHT(height)
{
	m_ActPtr->AddBoxFixture(width, height, 0.0);

	m_IsActive = false;

	b2Filter collisionFilter;
	collisionFilter.categoryBits = filterCategoryBits;
	collisionFilter.maskBits = Level::LEVEL | Level::PLAYER | Level::BLOCK | Level::YOSHI_TOUNGE;
	m_ActPtr->SetCollisionFilter(collisionFilter);
}

Item::~Item() {}

void Item::Tick(double deltaTime)
{
	if (m_IsActive == false)
	{
		if (abs(m_LevelPtr->GetPlayer()->GetPosition().x - m_ActPtr->GetPosition().x) < MINIMUM_PLAYER_DISTANCE)
		{
			m_IsActive = true;
		}
	}
	else
	{
		if (abs(m_LevelPtr->GetPlayer()->GetPosition().x - m_ActPtr->GetPosition().x) >= MINIMUM_PLAYER_DISTANCE)
		{
			m_IsActive = false;
		}
	}
}

Item::Type Item::GetType()
{
	return m_Type;
}

void Item::SetPosition(DOUBLE2 newPos)
{
	m_ActPtr->SetPosition(newPos);
}

void Item::SetLinearVelocity(DOUBLE2 newVel)
{
	m_ActPtr->SetLinearVelocity(newVel);
}

std::string Item::TYPEToString(Type type)
{
	switch (type)
	{
	case Type::NONE: return "None";
	case Type::COIN: return "Coin";
	case Type::DRAGON_COIN: return "DragonCoin";
	case Type::BERRY: return "Berry";
	case Type::KOOPA_SHELL: return "KoopaShell";
	case Type::KEY: return "Key";
	case Type::KEYHOLE: return "Keyhole";
	case Type::P_SWITCH: return "PSwitch";
	case Type::ONE_UP_MUSHROOM: return "OneUpMushroom";
	case Type::THREE_UP_MOON: return "ThreeUpMoon";
	case Type::SUPER_MUSHROOM: return "SuperMushroom";
	case Type::FIRE_FLOWER: return "FireFlower";
	case Type::CAPE_FEATHER: return "CapeFeather";
	case Type::STAR: return "Star";
	case Type::POWER_BALLOON: return "PowerBalloon";
	case Type::PRIZE_BLOCK: return "PrizeBlock";
	case Type::MESSAGE_BLOCK: return "MessageBlock";
	case Type::ROTATING_BLOCK: return "RotatingBlock";
	case Type::EXCLAMATION_MARK_BLOCK: return "ExclamationMarkBlock";
	case Type::CLOUD_BLOCK: return "Cloud";
	case Type::GRAB_BLOCK: return "GrabBlock";
	case Type::BEANSTALK: return "Beanstalk";
	case Type::MIDWAY_GATE: return "MidwayGate";
	case Type::GOAL_GATE: return "GoalGate";
	default:
	{
		OutputDebugString(String("ERROR: Unhandled item type in Item::TYPEToString: ") + String(int(type)) + String("\n"));
		return "";
	}
	}
}

Item::Type Item::StringToTYPE(std::string string)
{
	if (!string.compare("None")) return Type::NONE;
	else if (!string.compare("Coin")) return Type::COIN;
	else if (!string.compare("DragonCoin")) return Type::DRAGON_COIN;
	else if (!string.compare("Berry")) return Type::BERRY;
	else if (!string.compare("KoopaShell")) return Type::KOOPA_SHELL;
	else if (!string.compare("Key")) return Type::KEY;
	else if (!string.compare("Keyhole")) return Type::KEYHOLE;
	else if (!string.compare("PSwitch")) return Type::P_SWITCH;
	else if (!string.compare("OneUpMushroom")) return Type::ONE_UP_MUSHROOM;
	else if (!string.compare("ThreeUpMoon")) return Type::THREE_UP_MOON;
	else if (!string.compare("SuperMushroom")) return Type::SUPER_MUSHROOM;
	else if (!string.compare("FireFlower")) return Type::FIRE_FLOWER;
	else if (!string.compare("CapeFeather")) return Type::CAPE_FEATHER;
	else if (!string.compare("Star")) return Type::STAR;
	else if (!string.compare("PowerBalloon")) return Type::POWER_BALLOON;
	else if (!string.compare("PrizeBlock")) return Type::PRIZE_BLOCK;
	else if (!string.compare("MessageBlock")) return Type::MESSAGE_BLOCK;
	else if (!string.compare("RotatingBlock")) return Type::ROTATING_BLOCK;
	else if (!string.compare("ExclamationMarkBlock")) return Type::EXCLAMATION_MARK_BLOCK;
	else if (!string.compare("Cloud")) return Type::CLOUD_BLOCK;
	else if (!string.compare("GrabBlock")) return Type::GRAB_BLOCK;
	else if (!string.compare("Beanstalk")) return Type::BEANSTALK;
	else if (!string.compare("MidwayGate")) return Type::MIDWAY_GATE;
	else if (!string.compare("GoalGate")) return Type::GOAL_GATE;
	else
	{
		OutputDebugString(String("ERROR: Unhandled item type in Item::TYPEToString: ") + String(string.c_str()) + String("\n"));
	}
	return Type::NONE;
}

bool Item::IsBlock()
{
	return (m_Type == Type::EXCLAMATION_MARK_BLOCK ||
			m_Type == Type::MESSAGE_BLOCK ||
			m_Type == Type::PRIZE_BLOCK ||
			m_Type == Type::ROTATING_BLOCK ||
			m_Type == Type::CLOUD_BLOCK ||
			m_Type == Type::GRAB_BLOCK);
}
