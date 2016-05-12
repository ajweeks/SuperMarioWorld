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
const int Yoshi::WALK_BASE_VEL = 5000;
const int Yoshi::RUN_BASE_VEL = 9500;
const int Yoshi::TOUNGE_VEL = 9000;

const float Yoshi::HATCHING_SECONDS_PER_FRAME = 0.6f;
const float Yoshi::WAITING_SECONDS_PER_FRAME = 0.3f;
const float Yoshi::WALKING_SECONDS_PER_FRAME = 0.03f;

Yoshi::Yoshi(DOUBLE2 position, Level* levelPtr) : 
	Entity(position, BodyType::DYNAMIC, levelPtr, ActorId::YOSHI, this)
{
	m_ActPtr->AddBoxFixture(GetWidth(), GetHeight(), 0.0);

	m_AnimInfo.secondsPerFrame = HATCHING_SECONDS_PER_FRAME;
	m_AnimationState = ANIMATION_STATE::EGG;

	m_SpriteSheetPtr = SpriteSheetManager::smallYoshiPtr;

	m_HatchingTimer = CountdownTimer(80);
	m_HatchingTimer.Start();

	m_ChangingDirectionsTimer = CountdownTimer(15);
	m_ToungeTimer = CountdownTimer(35);

	m_ItemsEaten = 0;

	m_DirFacing = Direction::RIGHT;
	m_DirFacingLastFrame = Direction::RIGHT;
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

	m_ChangingDirectionsTimer.Tick();

	if (m_HatchingTimer.Tick() && m_HatchingTimer.IsComplete())
	{
		m_AnimationState = ANIMATION_STATE::WAITING;
		m_SpriteSheetPtr = SpriteSheetManager::yoshiPtr;
		m_AnimInfo.secondsPerFrame = WAITING_SECONDS_PER_FRAME;
	}

	HandleKeyboardInput(deltaTime);

	if (m_IsToungeStuckOut)
	{
		m_ActToungePtr->SetPosition(m_ActToungePtr->GetPosition().x, m_ActPtr->GetPosition().y);

		if (m_ToungeTimer.FramesElapsed() > m_ToungeTimer.OriginalNumberOfFrames() / 2)
		{
			m_ActToungePtr->SetLinearVelocity(DOUBLE2(-m_DirFacing * TOUNGE_VEL * deltaTime, 0));
		}
	}
}

void Yoshi::HandleKeyboardInput(double deltaTime)
{
	double horizontalVel = 0.0;
	double verticalVel = 0.0;

	if (m_IsCarryingPlayer)
	{
		m_IsOnGround = CalculateOnGround();
		if (!m_WasOnGround && m_IsOnGround)
		{
			m_AnimationState = ANIMATION_STATE::WAITING;
		}

		if (m_PlayerPtr->IsDucking() == false)
		{
			if (GAME_ENGINE->IsKeyboardKeyDown(VK_LEFT))
			{
				m_AnimationState = ANIMATION_STATE::WALKING;
				m_DirFacing = Direction::LEFT;
				horizontalVel = WALK_BASE_VEL * deltaTime;
			}
			else if (GAME_ENGINE->IsKeyboardKeyDown(VK_RIGHT))
			{
				m_AnimationState = ANIMATION_STATE::WALKING;
				m_DirFacing = Direction::RIGHT;
				horizontalVel = WALK_BASE_VEL * deltaTime;
			}
			else if (m_AnimationState == ANIMATION_STATE::WALKING)
			{
				m_AnimationState = ANIMATION_STATE::WAITING;
			}

			if (GAME_ENGINE->IsKeyboardKeyPressed('A') || 
				GAME_ENGINE->IsKeyboardKeyPressed('S'))
			{
				if (m_AnimationState == ANIMATION_STATE::WALKING)
				{
					m_IsRunning = true;
					horizontalVel = RUN_BASE_VEL * deltaTime;
				}
				
				if (m_ItemInMouthType != Item::TYPE::NONE)
				{
					SpitOutItem();
				}
				else if (m_IsToungeStuckOut == false)
				{
					m_IsToungeStuckOut = true;
					m_ToungeTimer.Start();
					DOUBLE2 toungePos = m_ActPtr->GetPosition() + DOUBLE2(m_DirFacing * 4, 0);
					m_ActToungePtr = new PhysicsActor(toungePos, 0, BodyType::KINEMATIC);
					m_ActToungePtr->AddBoxFixture(10, 10);
					m_ActToungePtr->SetSensor(true);
					m_ActToungePtr->SetLinearVelocity(DOUBLE2(m_DirFacing * TOUNGE_VEL * deltaTime, 0));
					m_ActToungePtr->SetUserData(int(ActorId::YOSHI_TOUNGE));
					m_ActToungePtr->SetUserPointer(this);
				}
			}
		}
		if (m_IsOnGround &&
			m_DirFacing != m_DirFacingLastFrame)
		{
			DustParticle* dustParticlePtr = new DustParticle(m_ActPtr->GetPosition() + DOUBLE2(0, GetHeight() / 2));
			m_LevelPtr->AddParticle(dustParticlePtr);

			m_ChangingDirectionsTimer.Start();
		}

		if (m_IsOnGround && GAME_ENGINE->IsKeyboardKeyPressed('Z'))
		{
			m_AnimationState = ANIMATION_STATE::JUMPING;
			verticalVel = JUMP_VEL * deltaTime;
		}

		DOUBLE2 oldVel = m_ActPtr->GetLinearVelocity();
		DOUBLE2 newVel = oldVel;

		if (horizontalVel != 0.0)
		{
			if (m_DirFacing == Direction::RIGHT) newVel.x = horizontalVel;
			else newVel.x = -horizontalVel;
		}
		if (verticalVel != 0.0)
		{
			newVel.y = verticalVel;
		}

		m_ActPtr->SetLinearVelocity(newVel);

		m_DirFacingLastFrame = m_DirFacing;
		m_WasOnGround = m_IsOnGround;
	}
}

bool Yoshi::CalculateOnGround()
{
	DOUBLE2 point1 = m_ActPtr->GetPosition();
	DOUBLE2 point2 = m_ActPtr->GetPosition() + DOUBLE2(0, (GetHeight() / 2 + 3));
	DOUBLE2 intersection, normal;
	double fraction = -1.0;
	int collisionBits = Level::COLLIDE_WITH_LEVEL | Level::COLLIDE_WITH_BLOCKS;

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

	GAME_ENGINE->DrawString(String(AnimationStateToString().c_str()), centerX + 10, centerY);
	
	bool changingDirections = m_ChangingDirectionsTimer.IsActive();
	bool firstHalf = m_ChangingDirectionsTimer.FramesElapsed() < m_ChangingDirectionsTimer.OriginalNumberOfFrames()/2;
	if (!changingDirections) firstHalf = false;

	if ((m_DirFacing == Direction::LEFT && !changingDirections && !firstHalf) ||
		(m_DirFacing == Direction::RIGHT && changingDirections && firstHalf))
	{
		MATRIX3X2 matReflect = MATRIX3X2::CreateScalingMatrix(DOUBLE2(-1, 1));
		MATRIX3X2 matTranslate = MATRIX3X2::CreateTranslationMatrix(centerX, centerY);
		MATRIX3X2 matTranslateInverse = MATRIX3X2::CreateTranslationMatrix(-centerX, -centerY);
		GAME_ENGINE->SetWorldMatrix(matTranslateInverse * matReflect * matTranslate * matPrevWorld);
	}

	PaintAnimationFrame(centerX - WIDTH/2 + 10, centerY - HEIGHT/2 + 2);

	if (m_IsToungeStuckOut)
	{
		int srcX = 13;
		bool playerIsSmall = true;
		if (m_PlayerPtr != nullptr) playerIsSmall = (m_PlayerPtr->GetPowerupState() == Player::POWERUP_STATE::NORMAL);
		int srcY = playerIsSmall ? 0 : 1;
		m_SpriteSheetPtr->Paint(centerX - WIDTH / 2 + 35, centerY - HEIGHT / 2 + 2 - 5, srcX, srcY);
	}

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
		case ANIMATION_STATE::WALKING:
		{
			if (m_IsCarryingPlayer)
			{
				srcY = playerIsSmall ? 0 : 1;
				if (m_ChangingDirectionsTimer.IsActive())
				{
					srcX = 4;
				}
				else
				{
					srcX = 1 + m_AnimInfo.frameNumber;
				}
			}
			else
			{
				srcX = 0 + m_AnimInfo.frameNumber;
				srcY = 0;
			}
		} break;
		case ANIMATION_STATE::JUMPING:
		{
			srcX = 3;
			srcY = playerIsSmall ? 0 : 1;
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

void Yoshi::SetCarryingPlayer(bool carryingPlayer, Player* playerPtr)
{
	m_IsCarryingPlayer = carryingPlayer;
	m_PlayerPtr = playerPtr;

	if (carryingPlayer)
	{
		m_SpriteSheetPtr = SpriteSheetManager::yoshiWithMarioPtr;
		m_AnimInfo.secondsPerFrame = WALKING_SECONDS_PER_FRAME;
	}
	else
	{
		m_AnimInfo.secondsPerFrame = WAITING_SECONDS_PER_FRAME;

		m_SpriteSheetPtr = SpriteSheetManager::yoshiPtr;
		if (m_AnimationState == ANIMATION_STATE::JUMPING) 
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
	return m_AnimationState == ANIMATION_STATE::JUMPING ||
		   m_AnimationState == ANIMATION_STATE::FALLING;
}

std::string Yoshi::AnimationStateToString()
{
	switch (m_AnimationState)
	{
	case ANIMATION_STATE::EGG: return "Egg";
	case ANIMATION_STATE::BABY: return "Baby";
	case ANIMATION_STATE::WAITING: return "Waiting";
	case ANIMATION_STATE::WALKING: return "Walking";
	case ANIMATION_STATE::JUMPING: return "Jumping";
	case ANIMATION_STATE::FALLING: return "Falling";
	case ANIMATION_STATE::WILD: return "Wild";
	default: return "UNKNOWN!";
	}
}