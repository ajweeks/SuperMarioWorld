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
#include "SuperMushroom.h"

// STATIC INITIALIZATIONS
const int Yoshi::JUMP_VEL = -25000;
const int Yoshi::TONGUE_VEL = 130;

const int Yoshi::RUN_VEL = 5000;

const float Yoshi::HATCHING_SECONDS_PER_FRAME = 0.6f;
const float Yoshi::WAITING_SECONDS_PER_FRAME = 0.3f;
const float Yoshi::WALKING_SECONDS_PER_FRAME = 0.03f;
const float Yoshi::RUNNING_SECONDS_PER_FRAME = 0.015f;
const float Yoshi::TONGUE_STUCK_OUT_SECONDS_PER_FRAME = 0.14f;

Yoshi::Yoshi(DOUBLE2 position, Level* levelPtr) : 
	Entity(position, BodyType::DYNAMIC, levelPtr, ActorId::YOSHI, this)
{
	m_ActPtr->AddBoxFixture(GetWidth(), GetHeight(), 0.0);

	b2Filter bodyCollisionFilter;
	bodyCollisionFilter.categoryBits = Level::YOSHI;
	bodyCollisionFilter.maskBits = Level::LEVEL | Level::BLOCK;
	m_ActPtr->SetCollisionFilter(bodyCollisionFilter);

	// NOTE: The tounge actor can _**NOT**_ have a body type of kinematic,
	// even though that is the most intuitive. This is because kinematic actors 
	// will not collide with static actors other (Berries)!
	m_ActToungePtr = new PhysicsActor(m_ActPtr->GetPosition(), 0, BodyType::DYNAMIC);
	m_ActToungePtr->SetGravityScale(0.0);
	m_ActToungePtr->AddBoxFixture(8, 8);
	m_ActToungePtr->SetUserData(int(ActorId::YOSHI_TOUNGE));
	m_ActToungePtr->SetUserPointer(this);
	m_ActToungePtr->SetFixedRotation(true);

	b2Filter toungeCollisionFilter;
	toungeCollisionFilter.categoryBits = Level::YOSHI_TOUNGE;		// I am Yoshi's tounge
	toungeCollisionFilter.maskBits = Level::BERRY | Level::ENEMY | Level::ITEM; // I collide with berries and enemies
	m_ActToungePtr->SetCollisionFilter(toungeCollisionFilter);


	m_AnimInfo.secondsPerFrame = HATCHING_SECONDS_PER_FRAME;
	m_AnimationState = AnimationState::EGG;

	m_SpriteSheetPtr = SpriteSheetManager::smallYoshiPtr;

	m_TongueTimer = CountdownTimer(35);
	m_HatchingTimer = CountdownTimer(80);
	m_HatchingTimer.Start();

	m_DirFacing = Direction::RIGHT;

	m_ItemsEaten = 0;

	SoundManager::PlaySoundEffect(SoundManager::Sound::YOSHI_SPAWN);
	SoundManager::PlaySoundEffect(SoundManager::Sound::YOSHI_EGG_BREAK); // LATER: add delay here?
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

	if (m_ShouldSpawnMushroom)
	{
		m_ShouldSpawnMushroom = false;
		SoundManager::PlaySoundEffect(SoundManager::Sound::YOSHI_EGG_BREAK);

		SuperMushroom* superMushroomPtr = new SuperMushroom(m_ActPtr->GetPosition(), m_LevelPtr, -m_DirFacing);
		m_LevelPtr->AddItem(superMushroomPtr);
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
	if (m_AnimationState == AnimationState::WAITING)
	{
		m_AnimInfo.frameNumber %= 3;
	}
	else
	{
		m_AnimInfo.frameNumber %= 2;
	}

	if (m_TongueTimer.Tick() && m_TongueTimer.IsComplete())
	{
		m_TongueXVel = 0.0;
		m_TongueLength = 0;
		m_IsTongueStuckOut = false;
	}

	if (m_HatchingTimer.Tick() && m_HatchingTimer.IsComplete())
	{
		m_AnimationState = AnimationState::WAITING;
		m_SpriteSheetPtr = SpriteSheetManager::yoshiPtr;
		m_AnimInfo.secondsPerFrame = WAITING_SECONDS_PER_FRAME;
	}

	if (m_IsCarryingPlayer)
	{
		HandleKeyboardInput(deltaTime);
	}
	else if (m_AnimationState == AnimationState::WILD)
	{
		UpdatePosition(deltaTime);
	}

	if (m_IsTongueStuckOut)
	{
		if (m_TongueTimer.FramesElapsed() == m_TongueTimer.OriginalNumberOfFrames() / 2)
		{
			m_TongueXVel = -m_TongueXVel;
		}
		
		m_TongueLength += m_TongueXVel * deltaTime;
	}
	double yo = 0.0, xo = 0.0;
	if (m_IsTongueStuckOut == false)
	{
		yo = -9;
		xo = 3;
	}
	m_ActToungePtr->SetPosition(m_ActPtr->GetPosition() + DOUBLE2(m_TongueLength + xo, yo));
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

	PaintAnimationFrame(centerX - WIDTH/2 + 10, centerY - HEIGHT/2 + 7);

	// LATER: Figure out a way to paint yoshi's tounge when the player isn't riding him

	GAME_ENGINE->SetWorldMatrix(matPrevWorld);
}

void Yoshi::PaintAnimationFrame(double left, double top)
{
	int srcX = 0, srcY = 0;
	bool playerIsSmall = false;

	if (m_PlayerPtr != nullptr) playerIsSmall = (m_PlayerPtr->GetPowerupState() == Player::PowerupState::NORMAL);

	if (m_IsTongueStuckOut)
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
		case AnimationState::EGG:
		{
			srcX = 0 + m_AnimInfo.frameNumber;
			srcY = 1;
		} break;
		case AnimationState::BABY:
		{
			srcX = 0 + m_AnimInfo.frameNumber;
			srcY = 0;
		} break;
		case AnimationState::WILD:
		{
			srcX = 0 + m_AnimInfo.frameNumber;
			srcY = 0;
		} break;
		case AnimationState::WAITING:
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
		case AnimationState::FALLING:
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
	m_AnimationState = AnimationState::WILD;
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
			m_AnimationState = AnimationState::FALLING;
			m_ActPtr->SetLinearVelocity(DOUBLE2(m_ActPtr->GetLinearVelocity().x, 0));
		}
		else
		{
			m_AnimationState = AnimationState::WAITING;
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
	case Item::Type::BERRY:
	{
		SwallowItem();
	} break;
	case Item::Type::KOOPA_SHELL:
	{
		if (m_ItemInMouthType == Item::Type::NONE)
		{
			m_ItemInMouthType = Item::Type::KOOPA_SHELL;
		}
	} break;
	}

	m_ItemToBeRemovedPtr = itemPtr;
}

void Yoshi::EatEnemy(Enemy* enemyPtr)
{
	switch (enemyPtr->GetType())
	{
	case Enemy::Type::KOOPA_TROOPA:
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
	case Enemy::Type::CHARGIN_CHUCK:
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

		m_ShouldSpawnMushroom = true;
	}
	else
	{
		SoundManager::PlaySoundEffect(SoundManager::Sound::YOSHI_SWALLOW);
	}
}

void Yoshi::SpitOutItem()
{
	if (m_ItemInMouthType != Item::Type::NONE)
	{
		switch (m_ItemInMouthType)
		{
		case Item::Type::KOOPA_SHELL:
		{
			KoopaShell* koopaShellPtr = new KoopaShell(m_ActPtr->GetPosition(), m_LevelPtr, Colour::GREEN);
			koopaShellPtr->SetMoving(true);
			m_LevelPtr->AddItem(koopaShellPtr);

			m_ItemInMouthType = Item::Type::NONE;

			// TODO: if red, spit fire ballz
			//SoundManager::PlaySoundEffect(SoundManager::Sound::YOSHI_FIRE_SPIT);

			SoundManager::PlaySoundEffect(SoundManager::Sound::YOSHI_SPIT);
		} break;
		}
	}
}

void Yoshi::StickTongueOut(double deltaTime)
{
	m_IsTongueStuckOut = true;
	m_TongueTimer.Start();
	
	m_TongueXVel = TONGUE_VEL * m_DirFacing;
}

bool Yoshi::IsTongueStuckOut()
{
	return m_IsTongueStuckOut;
}

const CountdownTimer Yoshi::GetTongueTimer()
{
	return m_TongueTimer;
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
	return m_AnimationState == AnimationState::EGG || m_AnimationState == AnimationState::BABY;
}

int Yoshi::GetDirectionFacing()
{
	return m_DirFacing;
}

Yoshi::AnimationState Yoshi::GetAnimationState()
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

double Yoshi::GetTongueLength()
{
	return m_TongueLength;
}

std::string Yoshi::AnimationStateToString()
{
	switch (m_AnimationState)
	{
	case AnimationState::EGG: return "Egg";
	case AnimationState::BABY: return "Baby";
	case AnimationState::WAITING: return "Waiting";
	case AnimationState::FALLING: return "Falling";
	case AnimationState::WILD: return "Wild";
	default: return "UNKNOWN!";
	}
}