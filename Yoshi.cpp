#include "stdafx.h"

#include "Yoshi.h"
#include "Game.h"
#include "Level.h"
#include "Player.h"
#include "Enemy.h"
#include "Item.h"
#include "KoopaTroopa.h"
#include "KoopaShell.h"

#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "DustParticle.h"

// STATIC INITIALIZATIONS
const int Yoshi::JUMP_VEL = -25000;
const int Yoshi::TOUNGE_VEL = 150;

const int Yoshi::RUN_VEL = 5000;

const float Yoshi::HATCHING_SECONDS_PER_FRAME = 0.6f;
const float Yoshi::WAITING_SECONDS_PER_FRAME = 0.3f;
const float Yoshi::WALKING_SECONDS_PER_FRAME = 0.03f;
const float Yoshi::RUNNING_SECONDS_PER_FRAME = 0.015f;
const float Yoshi::TOUNGE_STUCK_OUT_SECONDS_PER_FRAME = 0.14f;

Yoshi::Yoshi(DOUBLE2 position, Level* levelPtr) : 
	Entity(position, BodyType::DYNAMIC, levelPtr, ActorId::YOSHI, this)
{
	m_ActPtr->AddBoxFixture(GetWidth(), GetHeight(), 0.0);

	b2Filter collisionFilter;
	collisionFilter.categoryBits = Level::YOSHI;
	collisionFilter.maskBits = Level::LEVEL | Level::BLOCK;
	m_ActPtr->SetCollisionFilter(collisionFilter);

	m_AnimInfo.secondsPerFrame = HATCHING_SECONDS_PER_FRAME;
	m_AnimationState = ANIMATION_STATE::EGG;

	m_SpriteSheetPtr = SpriteSheetManager::smallYoshiPtr;

	m_ToungeTimer = CountdownTimer(35);
	m_HatchingTimer = CountdownTimer(80);
	m_HatchingTimer.Start();

	m_DirFacing = Direction::RIGHT;

	m_ItemsEaten = 0;
}

Yoshi::~Yoshi()
{
	delete m_ActToungePtr;
}

void Yoshi::Tick(double deltaTime)
{
	if (m_ItemToBeRemovedPtr != nullptr)
	{
		m_LevelPtr->RemoveItem(m_ItemToBeRemovedPtr);
		m_ItemToBeRemovedPtr = nullptr;
	}
	if (m_EnemyToBeRemovedPtr != nullptr)
	{
		m_LevelPtr->RemoveEnemy(m_EnemyToBeRemovedPtr);
		m_EnemyToBeRemovedPtr = nullptr;
	}

	if (m_NeedsNewFixture)
	{
		double oldHalfHeight = GetHeight() / 2;

		b2Fixture* fixturePtr = m_ActPtr->GetBody()->GetFixtureList();
		while (fixturePtr != nullptr)
		{
			b2Fixture* nextFixturePtr = fixturePtr->GetNext();
			m_ActPtr->GetBody()->DestroyFixture(fixturePtr);
			fixturePtr = nextFixturePtr;
		}
		// If we are carrying the player, we don't need a new box fixture,
		// we will just use theirs
		if (m_IsCarryingPlayer)
		{
			m_NeedsNewFixture = false;
			m_ActPtr->SetActive(false);
			return;
		}
		m_ActPtr->SetActive(true);

		m_ActPtr->AddBoxFixture(GetWidth(), GetHeight(), 0.0, 0.02);

		double newHalfHeight = GetHeight() / 2;

		double newCenterY = m_ActPtr->GetPosition().y + (newHalfHeight - oldHalfHeight);
		m_ActPtr->SetPosition(DOUBLE2(m_ActPtr->GetPosition().x, newCenterY));

		m_NeedsNewFixture = false;
	}

	// Tick animations
	m_AnimInfo.Tick(deltaTime);
	if (m_AnimationState == ANIMATION_STATE::WAITING)
	{
		m_AnimInfo.frameNumber %= 3;
	}
	else
	{
		m_AnimInfo.frameNumber %= 2;
	}

	if (m_ToungeTimer.Tick() && m_ToungeTimer.IsComplete())
	{
		delete m_ActToungePtr;
		m_ActToungePtr = nullptr;

		m_IsToungeStuckOut = false;
	}

	if (m_HatchingTimer.Tick() && m_HatchingTimer.IsComplete())
	{
		m_AnimationState = ANIMATION_STATE::WAITING;
		m_SpriteSheetPtr = SpriteSheetManager::yoshiPtr;
		m_AnimInfo.secondsPerFrame = WAITING_SECONDS_PER_FRAME;
	}

	if (m_IsCarryingPlayer)
	{
		HandleKeyboardInput(deltaTime);
	}
	else if (m_AnimationState == ANIMATION_STATE::WILD)
	{
		UpdatePosition(deltaTime);
	}

	if (m_IsToungeStuckOut)
	{
		if (m_ToungeTimer.FramesElapsed() == m_ToungeTimer.OriginalNumberOfFrames() / 2)
		{
			m_ToungeXVel = -m_ToungeXVel;
		}
		
		m_ToungeLength += m_ToungeXVel * deltaTime;
		m_ActToungePtr->SetPosition(m_ActPtr->GetPosition() + DOUBLE2(m_ToungeLength, 0));
	}
}

void Yoshi::UpdatePosition(double deltaTime)
{
	DOUBLE2 prevVel = m_ActPtr->GetLinearVelocity();
	DOUBLE2 molePos = m_ActPtr->GetPosition();

	double newXVel = prevVel.x;

	// Just run until you hit an obstacle, then turn around
	DOUBLE2 point1 = m_ActPtr->GetPosition();
	DOUBLE2 point2 = m_ActPtr->GetPosition() + DOUBLE2(m_DirFacing * (GetWidth() / 2 + 2), 0);
	DOUBLE2 intersection, normal;
	double fraction = -1.0;
	int collisionBits = Level::LEVEL | Level::BLOCK;
	if (m_LevelPtr->Raycast(point1, point2, collisionBits, intersection, normal, fraction))
	{
		// Turn around
		m_DirFacing = -m_DirFacing;
	}

	// Prevent walking off the left side of the level
	if (m_DirFacing == Direction::LEFT && m_ActPtr->GetPosition().x < GetWidth())
	{
		m_DirFacing = -m_DirFacing;
	}

	newXVel = double(m_DirFacing) * RUN_VEL * deltaTime;

	m_ActPtr->SetLinearVelocity(DOUBLE2(newXVel, prevVel.y));
}

void Yoshi::HandleKeyboardInput(double deltaTime)
{
	m_IsOnGround = m_PlayerPtr->IsOnGround();
	m_ActPtr->SetPosition(m_PlayerPtr->GetPosition());
	m_DirFacing = m_PlayerPtr->GetDirectionFacing();
}

bool Yoshi::CalculateOnGround()
{
	DOUBLE2 point1 = m_ActPtr->GetPosition();
	DOUBLE2 point2 = m_ActPtr->GetPosition() + DOUBLE2(0, (GetHeight() / 2 + 3));
	DOUBLE2 intersection, normal;
	double fraction = -1.0;
	int collisionBits = Level::LEVEL | Level::BLOCK;

	if (m_LevelPtr->Raycast(point1, point2, collisionBits, intersection, normal, fraction))
	{
		return true;
	}

	return false;
}

void Yoshi::Paint()
{
	MATRIX3X2 matPrevWorld = GAME_ENGINE->GetWorldMatrix();

	double centerX = m_ActPtr->GetPosition().x;
	double centerY = m_ActPtr->GetPosition().y;

	if (m_DirFacing == Direction::LEFT)
	{
		MATRIX3X2 matReflect = MATRIX3X2::CreateScalingMatrix(DOUBLE2(-1, 1));
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(centerX, centerY);
		MATRIX3X2 matTranslateInverse = MATRIX3X2::CreateTranslationMatrix(-centerX, -centerY);
		GAME_ENGINE->SetWorldMatrix(matTranslateInverse * matReflect * matTranslate * matPrevWorld);
	}

	PaintAnimationFrame(centerX - WIDTH/2 + 10, centerY - HEIGHT/2 + 2);

	// LATER: Figure out a way to paint yoshi's tounge when the player isn't riding him

	GAME_ENGINE->SetWorldMatrix(matPrevWorld);
}

void Yoshi::PaintAnimationFrame(double left, double top)
{
	int srcX = 0, srcY = 0;
	bool playerIsSmall = false;

	if (m_PlayerPtr != nullptr) playerIsSmall = (m_PlayerPtr->GetPowerupState() == Player::POWERUP_STATE::NORMAL);

	if (m_IsToungeStuckOut)
	{
		if (m_IsCarryingPlayer)
		{
			srcX = 10 + m_AnimInfo.frameNumber;
			srcY = playerIsSmall ? 0 : 1;
		}
		else
		{
			srcX = 3;
			srcY = 0;
		}
	}
	else
	{
		switch (m_AnimationState)
		{
		case ANIMATION_STATE::EGG:
		{
			srcX = 0 + m_AnimInfo.frameNumber;
			srcY = 1;
		} break;
		case ANIMATION_STATE::BABY:
		{
			srcX = 0 + m_AnimInfo.frameNumber;
			srcY = 0;
		} break;
		case ANIMATION_STATE::WILD:
		{
			srcX = 0 + m_AnimInfo.frameNumber;
			srcY = 0;
		} break;
		case ANIMATION_STATE::WAITING:
		{
			if (m_IsCarryingPlayer)
			{
				srcY = playerIsSmall ? 0 : 1;
				if (m_PlayerPtr->IsDucking())
				{
					srcX = 5;
				}
				else
				{
					srcX = 1;
				}
			}
			else
			{
				srcX = 2 + m_AnimInfo.frameNumber;
				srcY = 0;
			}
		} break;
		case ANIMATION_STATE::FALLING:
		{
			if (m_IsCarryingPlayer)
			{
				srcX = 2;
				srcY = playerIsSmall ? 0 : 1;
			}
			else
			{
				srcX = 1;
				srcY = 0;
			}
		} break;
		}
	}

	m_SpriteSheetPtr->Paint(left, top - 5, srcX, srcY);
}

void Yoshi::RunWild()
{
	SetCarryingPlayer(false, nullptr);
	m_AnimationState = ANIMATION_STATE::WILD;
	m_AnimInfo.secondsPerFrame = RUNNING_SECONDS_PER_FRAME;
}

void Yoshi::SetCarryingPlayer(bool carryingPlayer, Player* playerPtr)
{
	if (carryingPlayer)
	{
		m_AnimInfo.secondsPerFrame = WALKING_SECONDS_PER_FRAME;
	}
	else
	{
		m_AnimInfo.secondsPerFrame = WAITING_SECONDS_PER_FRAME;

		m_SpriteSheetPtr = SpriteSheetManager::yoshiPtr;
		
		if (m_PlayerPtr->IsAirborne()) 
		{
			m_AnimationState = ANIMATION_STATE::FALLING;
			m_ActPtr->SetLinearVelocity(DOUBLE2(m_ActPtr->GetLinearVelocity().x, 0));
		}
		else
		{
			m_AnimationState = ANIMATION_STATE::WAITING;
		}

		m_AnimInfo.frameNumber = 0;
	}

	m_PlayerPtr = playerPtr;
	m_IsCarryingPlayer = carryingPlayer;
	m_NeedsNewFixture = true;
}

void Yoshi::EatItem(Item* itemPtr)
{
	switch (itemPtr->GetType())
	{
	case Item::TYPE::BERRY:
	{
		SwallowItem();
	} break;
	case Item::TYPE::KOOPA_SHELL:
	{
		if (m_ItemInMouthType == Item::TYPE::NONE)
		{
			m_ItemInMouthType = Item::TYPE::KOOPA_SHELL;
		}
	} break;
	}

	m_ItemToBeRemovedPtr = itemPtr;
}

void Yoshi::EatEnemy(Enemy* enemyPtr)
{
	switch (enemyPtr->GetType())
	{
	case Enemy::TYPE::KOOPA_TROOPA:
	{
		KoopaTroopa* koopaPtr = (KoopaTroopa*)enemyPtr;
		if (koopaPtr->IsShelless())
		{
			SwallowItem();
		}
		else
		{
			//m_ItemInMouthPtr = new KoopaShell(m_ActPtr->GetPosition(), m_LevelPtr, koopaPtr->GetColour());
		}
	} break;
	case Enemy::TYPE::CHARGIN_CHUCK:
	{
		return; // The player can't eat these dudes
	} break;
	}

	m_EnemyToBeRemovedPtr = enemyPtr;
}

void Yoshi::SwallowItem()
{
	++m_ItemsEaten;
	if (m_ItemsEaten > MAX_ITEMS_EATEN)
	{
		m_ItemsEaten = 0;

		// TODO: Set flag for spawning a super mushroom here
	}
}

void Yoshi::SpitOutItem()
{
	if (m_ItemInMouthType != Item::TYPE::NONE)
	{
		switch (m_ItemInMouthType)
		{
		case Item::TYPE::KOOPA_SHELL:
		{
			KoopaShell* koopaShellPtr = new KoopaShell(m_ActPtr->GetPosition(), m_LevelPtr, COLOUR::GREEN);
			koopaShellPtr->SetMoving(true);
			m_LevelPtr->AddItem(koopaShellPtr);

			m_ItemInMouthType = Item::TYPE::NONE;
		} break;
		}
	}
}

void Yoshi::StickToungeOut(double deltaTime)
{
	m_IsToungeStuckOut = true;
	m_ToungeTimer.Start();
	
	m_ToungeXVel = TOUNGE_VEL * m_DirFacing;
	m_ToungeLength = m_DirFacing * 4;

	DOUBLE2 toungePos = m_ActPtr->GetPosition() + DOUBLE2(m_ToungeLength, 0);
	// NOTE: The tounge actor can _**NOT**_ have a body type of kinematic,
	// even though that is the most intuitive. This is because kinematic actors 
	// will not collide with static actors other (Berries)!
	m_ActToungePtr = new PhysicsActor(toungePos, 0, BodyType::DYNAMIC);
	m_ActToungePtr->SetGravityScale(0.0);
	m_ActToungePtr->AddBoxFixture(10, 10);
	m_ActToungePtr->SetUserData(int(ActorId::YOSHI_TOUNGE));
	m_ActToungePtr->SetUserPointer(this);

	b2Filter collisionFilter;
	collisionFilter.categoryBits = Level::YOSHI_TOUNGE;		// I am Yoshi's tounge
	collisionFilter.maskBits = Level::BERRY | Level::ENEMY; // I collide with berries and enemies
	m_ActToungePtr->SetCollisionFilter(collisionFilter);
}

bool Yoshi::IsToungeStuckOut()
{
	return m_IsToungeStuckOut;
}

const CountdownTimer Yoshi::GetToungeTimer()
{
	return m_ToungeTimer;
}

int Yoshi::GetWidth()
{
	return WIDTH;
}

int Yoshi::GetHeight()
{
	return HEIGHT;
}

bool Yoshi::IsHatching()
{
	return m_AnimationState == ANIMATION_STATE::EGG || m_AnimationState == ANIMATION_STATE::BABY;
}

int Yoshi::GetDirectionFacing()
{
	return m_DirFacing;
}

Yoshi::ANIMATION_STATE Yoshi::GetAnimationState()
{
	return m_AnimationState;
}

bool Yoshi::IsAirborne()
{
	return !m_IsOnGround;
}

void Yoshi::SetPaused(bool paused)
{
	m_ActPtr->SetActive(!paused);

	if (m_ActToungePtr != nullptr)
	{
		m_ActToungePtr->SetActive(!paused);
	}
}

std::string Yoshi::AnimationStateToString()
{
	switch (m_AnimationState)
	{
	case ANIMATION_STATE::EGG: return "Egg";
	case ANIMATION_STATE::BABY: return "Baby";
	case ANIMATION_STATE::WAITING: return "Waiting";
	case ANIMATION_STATE::FALLING: return "Falling";
	case ANIMATION_STATE::WILD: return "Wild";
	default: return "UNKNOWN!";
	}
}