#include "stdafx.h"
#include "Enemy.h"

Enemy::Enemy(TYPE type, DOUBLE2& posRef, double width, double height, BodyType bodyType, Level* levelPtr, void* userPointer) :
	Entity(posRef, bodyType, levelPtr, userPointer),
	m_Type(type)
{
	m_ActPtr->AddBoxFixture(width, height, 0.0);
	m_ActPtr->SetFixedRotation(true);
	m_ActPtr->SetUserData(int(ActorId::ENEMY));
}

Enemy::~Enemy()
{
}

Enemy::TYPE Enemy::GetType()
{
	return m_Type;
}

std::string Enemy::TYPEToString(TYPE type)
{
	switch (type)
	{
	case TYPE::NONE: return "None";
	case TYPE::KOOPA_TROOPA: return "KoopaTroopa";
	case TYPE::CHARGIN_CHUCK: return "CharginChuck";
	case TYPE::PIRHANA_PLANT: return "PirhanaPlant";
	case TYPE::MONTY_MOLE: return "MontyMole";
	default:
	{
		OutputDebugString(String("ERROR: Unhandled type passed to Enemy::TYPEToString\n"));
		return "";
	}
	}
}

Enemy::TYPE Enemy::StringToTYPE(std::string string)
{
	if (!string.compare("None")) return TYPE::NONE;
	else if (!string.compare("KoopaTroopa")) return TYPE::KOOPA_TROOPA;
	else if (!string.compare("CharginChuck")) return TYPE::CHARGIN_CHUCK;
	else if (!string.compare("PirhanaPlant")) return TYPE::PIRHANA_PLANT;
	else if (!string.compare("MontyMole")) return TYPE::MONTY_MOLE;
	else
	{
		OutputDebugString(String("ERROR: Unhandled type passed to Enemy::TYPEToString\n"));
		return TYPE::NONE;
	}
}