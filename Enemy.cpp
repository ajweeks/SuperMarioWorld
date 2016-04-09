#include "stdafx.h"
#include "Enemy.h"

Enemy::Enemy(TYPE type, DOUBLE2& posRef, double width, double height, SpriteSheet* spriteSheetPtr, BodyType bodyType, Level* levelPtr, void* userPointer) :
	Entity(posRef, spriteSheetPtr, bodyType, levelPtr, userPointer),
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