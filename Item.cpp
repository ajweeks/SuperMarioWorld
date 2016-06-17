#include "stdafx.h"

#include "Item.h"
#include "SpriteSheetManager.h"
#include "Block.h"
#include "Level.h"
#include "Game.h"
#include "Player.h"

#include "Coin.h"
#include "KoopaShell.h"
#include "GrabBlock.h"
#include "PSwitch.h"

const int Item::MINIMUM_PLAYER_DISTANCE = int(Game::WIDTH * (4.0 / 5.0));

Item::Item(DOUBLE2 topLeft, Type type, Level* levelPtr, int filterCategoryBits, BodyType bodyType, int width, int height) :
	Entity(topLeft + DOUBLE2(width / 2, height / 2), bodyType, levelPtr, ActorId::ITEM, this),
	m_Type(type), WIDTH(width), HEIGHT(height), m_SpawningPosition(topLeft + DOUBLE2(width / 2, height / 2))
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

bool Item::IsBlock()
{
	return (m_Type == Type::EXCLAMATION_MARK_BLOCK ||
			m_Type == Type::MESSAGE_BLOCK ||
			m_Type == Type::PRIZE_BLOCK ||
			m_Type == Type::ROTATING_BLOCK ||
			m_Type == Type::CLOUD_BLOCK ||
			m_Type == Type::GRAB_BLOCK ||
			(m_Type == Type::COIN && ((Coin*)this)->IsBlock()));
}

std::string Item::ItemToString(Item* itemPtr)
{
	switch (itemPtr->GetType())
	{
	case Item::Type::KOOPA_SHELL:
	{
		KoopaShell* koopaShellPtr = (KoopaShell*)itemPtr;
		std::string result = "KoopaShell";
		if (koopaShellPtr->GetColour() == Colour::RED) result += ",Red";
		else if (koopaShellPtr->GetColour() == Colour::GREEN) result += ",Green";
		return result;
	} break;
	// These items don't need any extra info
	case Type::P_SWITCH:return "PSwitch";
	case Type::GRAB_BLOCK: return "GrabBlock";
	case Type::COIN: return "Coin";
	case Type::DRAGON_COIN: return "DragonCoin";
	case Type::BERRY: return "Berry";
	case Type::KEY: return "Key";
	case Type::KEYHOLE: return "Keyhole";
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
	case Type::BEANSTALK: return "Beanstalk";
	case Type::MIDWAY_GATE: return "MidwayGate";
	case Type::GOAL_GATE: return "GoalGate";
	case Type::FIREBALL: return "Fireball";
	case Type::NONE:  // fallthrough
	default: return "None";
	}
	return "";
}

Item* Item::StringToItem(const std::string& itemString, DOUBLE2 itemPos, Level* levelPtr)
{
	const int comma1 = itemString.find(',');
	const std::string type = itemString.substr(0, comma1);
	if (!type.compare("KoopaShell"))
	{
		const int comma2 = itemString.find(',', comma1 + 1);
		const std::string colourString = itemString.substr(comma1 + 1, comma2 - comma1);
		Colour koopaShellColour = Colour::RED;
		if (!colourString.compare("Red")) koopaShellColour = Colour::RED;
		else if (!colourString.compare("Green")) koopaShellColour = Colour::GREEN;

		return new KoopaShell(itemPos, levelPtr, koopaShellColour);
	}
	else if (!type.compare("GrabBlock")) return new GrabBlock(itemPos, levelPtr);
	else if (!type.compare("PSwitch")) return new PSwitch(itemPos, levelPtr);
	else if (!itemString.compare("None")) return nullptr;
	else
	{
		return nullptr;
	}
}

Item::Type Item::StringToTYPE(std::string typeString)
{
	if (!typeString.compare("KoopaShell")) return Type::KOOPA_SHELL;
	else if (!typeString.compare("PSwitch")) return Type::P_SWITCH;
	else if (!typeString.compare("GrabBlock")) return Type::GRAB_BLOCK;
	else if (!typeString.compare("Coin")) return Type::COIN;
	else if (!typeString.compare("DragonCoin")) return Type::DRAGON_COIN;
	else if (!typeString.compare("Berry")) return Type::BERRY;
	else if (!typeString.compare("KoopaShell")) return Type::KOOPA_SHELL;
	else if (!typeString.compare("Key")) return Type::KEY;
	else if (!typeString.compare("Keyhole")) return Type::KEYHOLE;
	else if (!typeString.compare("PSwitch")) return Type::P_SWITCH;
	else if (!typeString.compare("OneUpMushroom")) return Type::ONE_UP_MUSHROOM;
	else if (!typeString.compare("ThreeUpMoon")) return Type::THREE_UP_MOON;
	else if (!typeString.compare("SuperMushroom")) return Type::SUPER_MUSHROOM;
	else if (!typeString.compare("FireFlower")) return Type::FIRE_FLOWER;
	else if (!typeString.compare("CapeFeather")) return Type::CAPE_FEATHER;
	else if (!typeString.compare("Star")) return Type::STAR;
	else if (!typeString.compare("PowerBalloon")) return Type::POWER_BALLOON;
	else if (!typeString.compare("PrizeBlock")) return Type::PRIZE_BLOCK;
	else if (!typeString.compare("MessageBlock")) return Type::MESSAGE_BLOCK;
	else if (!typeString.compare("RotatingBlock")) return Type::ROTATING_BLOCK;
	else if (!typeString.compare("ExclamationMarkBlock")) return Type::EXCLAMATION_MARK_BLOCK;
	else if (!typeString.compare("Cloud")) return Type::CLOUD_BLOCK;
	else if (!typeString.compare("GrabBlock")) return Type::GRAB_BLOCK;
	else if (!typeString.compare("Beanstalk")) return Type::BEANSTALK;
	else if (!typeString.compare("MidwayGate")) return Type::MIDWAY_GATE;
	else if (!typeString.compare("GoalGate")) return Type::GOAL_GATE;
	else if (!typeString.compare("None")) return Type::NONE;
	else
	{
		OutputDebugString(String("ERROR: Couldn't read item string in Player::ParseItemString: \"") + String(typeString.c_str()) + String("\"\n"));
		return Type::NONE;
	}
}

std::string Item::TYPEToString(Type type)
{
	switch (type)
	{
	case Item::Type::KOOPA_SHELL: return "KoopaShell";
	case Type::P_SWITCH:return "PSwitch";
	case Type::GRAB_BLOCK: return "GrabBlock";
	case Type::COIN: return "Coin";
	case Type::DRAGON_COIN: return "DragonCoin";
	case Type::BERRY: return "Berry";
	case Type::KEY: return "Key";
	case Type::KEYHOLE: return "Keyhole";
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
	case Type::BEANSTALK: return "Beanstalk";
	case Type::MIDWAY_GATE: return "MidwayGate";
	case Type::GOAL_GATE: return "GoalGate";
	case Type::FIREBALL: return "Fireball";
	case Type::NONE: return "None";
	default: return "";
	}
}
