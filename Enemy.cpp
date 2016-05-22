#include "stdafx.h"

#include "Enemy.h"
#include "Level.h"

Enemy::Enemy(Type type, DOUBLE2& posRef, double width, double height, BodyType bodyType, Level* levelPtr, void* userPointer) :
	Entity(posRef, bodyType, levelPtr, ActorId::ENEMY, userPointer),
	m_Type(type)
{
	m_ActPtr->AddBoxFixture(width, height, 0.0);
	b2Filter collisionFilter;
	collisionFilter.categoryBits = Level::ENEMY;
	collisionFilter.maskBits = Level::LEVEL | Level::ENEMY | Level::BLOCK | Level::PLAYER | Level::SHELL | Level::YOSHI_TOUNGE;
	m_ActPtr->SetCollisionFilter(collisionFilter);
}

Enemy::~Enemy()
{
}

Enemy::Type Enemy::GetType()
{
	return m_Type;
}

std::string Enemy::TYPEToString(Type type)
{
	switch (type)
	{
	case Type::NONE: return "None";
	case Type::KOOPA_TROOPA: return "KoopaTroopa";
	case Type::CHARGIN_CHUCK: return "CharginChuck";
	case Type::PIRHANA_PLANT: return "PirhanaPlant";
	case Type::MONTY_MOLE: return "MontyMole";
	default:
	{
		OutputDebugString(String("ERROR: Unhandled type passed to Enemy::TYPEToString\n"));
		return "";
	}
	}
}

Enemy::Type Enemy::StringToTYPE(std::string string)
{
	if (!string.compare("None")) return Type::NONE;
	else if (!string.compare("KoopaTroopa")) return Type::KOOPA_TROOPA;
	else if (!string.compare("CharginChuck")) return Type::CHARGIN_CHUCK;
	else if (!string.compare("PirhanaPlant")) return Type::PIRHANA_PLANT;
	else if (!string.compare("MontyMole")) return Type::MONTY_MOLE;
	else
	{
		OutputDebugString(String("ERROR: Unhandled type passed to Enemy::TYPEToString\n"));
		return Type::NONE;
	}
}