#include "stdafx.h"

#include "Fireball.h"
#include "Game.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "Player.h"

const double Fireball::VERTICAL_VEL = 25;
const double Fireball::HORIZONTAL_VEL = 140;

Fireball::Fireball(DOUBLE2 topLeft, Level* levelPtr, int directionMoving) : 
	Item(topLeft, Type::FIREBALL, levelPtr, Level::ITEM, BodyType::KINEMATIC, WIDTH, HEIGHT),
	m_DirMoving(directionMoving)
{
	const double horizontalVel = HORIZONTAL_VEL * m_DirMoving;

	// TOP
	m_ActTopBallPtr = new PhysicsActor(topLeft + DOUBLE2(WIDTH / 2, HEIGHT/ 2), 0.0, BodyType::KINEMATIC);
	m_ActTopBallPtr->SetUserData(int(ActorId::ITEM));
	m_ActTopBallPtr->SetUserPointer(this);
	m_ActTopBallPtr->SetLinearVelocity(DOUBLE2(horizontalVel, -VERTICAL_VEL));
	m_ActTopBallPtr->SetFixedRotation(true);
	m_ActTopBallPtr->AddBoxFixture(WIDTH, HEIGHT, 0.0);

	// MIDDLE
	m_ActPtr->SetLinearVelocity(DOUBLE2(horizontalVel + 12 * m_DirMoving, 0));

	// BOTTOM
	m_ActBtmBallPtr = new PhysicsActor(topLeft + DOUBLE2(WIDTH / 2, HEIGHT/ 2), 0.0, BodyType::KINEMATIC);
	m_ActBtmBallPtr->SetUserData(int(ActorId::ITEM));
	m_ActBtmBallPtr->SetUserPointer(this);
	m_ActBtmBallPtr->SetLinearVelocity(DOUBLE2(horizontalVel, VERTICAL_VEL));
	m_ActBtmBallPtr->SetFixedRotation(true);
	m_ActBtmBallPtr->AddBoxFixture(WIDTH, HEIGHT, 0.0);

	m_IsActive = true;

	b2Filter collisionFilter;
	collisionFilter.categoryBits = Level::FIREBALL;
	collisionFilter.maskBits = Level::ENEMY;
	m_ActPtr->SetCollisionFilter(collisionFilter);
	m_ActTopBallPtr->SetCollisionFilter(collisionFilter);
	m_ActBtmBallPtr->SetCollisionFilter(collisionFilter);

	m_AnimInfo.secondsPerFrame = 0.05;
}

Fireball::~Fireball()
{
	delete m_ActTopBallPtr;
	delete m_ActBtmBallPtr;
}

void Fireball::Tick(double deltaTime)
{ 
	m_AnimInfo.Tick(deltaTime);
	m_AnimInfo.frameNumber %= 2;

	const DOUBLE2 playerPos = m_LevelPtr->GetPlayer()->GetPosition();
	const double dX = abs(m_ActPtr->GetPosition().x - playerPos.x);
	if (dX > Game::WIDTH) m_LevelPtr->RemoveItem(this);
}

void Fireball::Paint()
{
	MATRIX3X2 matWorldPrev = GAME_ENGINE->GetWorldMatrix();

	SpriteSheet* spriteSheetPtr = SpriteSheetManager::GetSpriteSheetPtr(SpriteSheetManager::GENERAL_TILES);

	const int srcX = 1 + m_AnimInfo.frameNumber;
	const int srcY = 13;

	int centerX = int(m_ActTopBallPtr->GetPosition().x);
	int centerY = int(m_ActTopBallPtr->GetPosition().y);
	if (m_DirMoving == Direction::LEFT)
	{
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(centerX, centerY);
		MATRIX3X2 matReflect = MATRIX3X2::CreateScalingMatrix(-1, 1);
		GAME_ENGINE->SetWorldMatrix(matTranslate.Inverse() * matReflect * matTranslate);
	}
	spriteSheetPtr->Paint(centerX, centerY, srcX, srcY);

	centerX = int(m_ActBtmBallPtr->GetPosition().x);
	centerY = int(m_ActBtmBallPtr->GetPosition().y);
	if (m_DirMoving == Direction::LEFT)
	{
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(centerX, centerY);
		MATRIX3X2 matReflect = MATRIX3X2::CreateScalingMatrix(-1, 1);
		GAME_ENGINE->SetWorldMatrix(matTranslate.Inverse() * matReflect * matTranslate);
	}
	spriteSheetPtr->Paint(centerX, centerY, srcX, srcY);

	centerX = int(m_ActPtr->GetPosition().x);
	centerY = int(m_ActPtr->GetPosition().y);
	if (m_DirMoving == Direction::LEFT)
	{
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(centerX, centerY);
		MATRIX3X2 matReflect = MATRIX3X2::CreateScalingMatrix(-1, 1);
		GAME_ENGINE->SetWorldMatrix(matTranslate.Inverse() * matReflect * matTranslate);
	}
	spriteSheetPtr->Paint(centerX, centerY, srcX, srcY);

	GAME_ENGINE->SetWorldMatrix(matWorldPrev);
}

void Fireball::SetPaused(bool paused)
{
	m_ActTopBallPtr->SetActive(!paused);
	m_ActPtr->SetActive(!paused);
	m_ActBtmBallPtr->SetActive(!paused);
}
