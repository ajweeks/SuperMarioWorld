#include "stdafx.h"

#include "Game.h"
#include "Level.h"
#include "Camera.h"
#include "LevelData.h"
#include "SpriteSheetManager.h"
#include "SoundManager.h"
#include "Particle.h"
#include "ParticleManager.h"
#include "Player.h"
#include "KoopaTroopa.h"

#define GAME_ENGINE (GameEngine::GetSingleton())

Level::Level()
{
	// TODO: Make player extend Entity
	m_PlayerPtr = new Player(this);

	m_ActLevelPtr = new PhysicsActor(DOUBLE2(0, 0), 0, BodyType::STATIC);
	m_ActLevelPtr->AddSVGFixture(String("Resources/levels/01/Level01hit.svg"), 0.0);
	m_ActLevelPtr->AddContactListener(this);
	m_ActLevelPtr->SetUserData(int(ActorId::LEVEL));

	SpriteSheetManager::levelOneForegroundPtr->SetTransparencyColor(COLOR(255,0,255));

	m_Width = SpriteSheetManager::levelOneForegroundPtr->GetWidth();
	m_Height = SpriteSheetManager::levelOneForegroundPtr->GetHeight();

	m_CameraPtr = new Camera(Game::WIDTH, Game::HEIGHT, this);

	m_ParticleManagerPtr = new ParticleManager();

	Reset();
}

Level::~Level()
{
	delete m_ActLevelPtr;
	delete m_PlayerPtr;
	delete m_CameraPtr;
	delete m_ParticleManagerPtr;
}

void Level::Reset()
{
	m_PlayerPtr->Reset();
	m_CameraPtr->Reset();

	m_Paused = false;
	m_ShowingMessage = false;

	m_TotalTime = 400; // This changes for every level, TODO: Put this info in the level save
	m_SecondsElapsed = 0.0;

	m_LevelDataPtr->GenerateLevelData(1, this);
	ReadLevelData(1);

	m_ParticleManagerPtr->Reset();

	SoundManager::RestartSongs();

	SoundManager::PlaySong(SoundManager::SONG::OVERWORLD_BGM);
}

void Level::ReadLevelData(int levelIndex)
{
	m_LevelDataPtr = LevelData::GetLevelData(levelIndex, this);

	std::vector<Platform*> platformsData = m_LevelDataPtr->GetPlatforms();
	std::vector<Pipe*> pipesData = m_LevelDataPtr->GetPipes();
	std::vector<Item*> itemsData = m_LevelDataPtr->GetItems();
	std::vector<Enemy*> enemiesData = m_LevelDataPtr->GetEnemies();

	for (size_t i = 0; i < platformsData.size(); ++i)
	{
		platformsData[i]->AddContactListener(this);
	}
	for (size_t i = 0; i < pipesData.size(); ++i)
	{
		pipesData[i]->AddContactListener(this);
	}
	for (size_t i = 0; i < itemsData.size(); ++i)
	{
		itemsData[i]->AddContactListener(this);
	}
	for (size_t i = 0; i < enemiesData.size(); ++i)
	{
		enemiesData[i]->AddContactListener(this);
	}
}

void Level::Tick(double deltaTime)
{
	if (m_ShowingMessage)
	{
		if (GAME_ENGINE->IsKeyboardKeyPressed('A') ||
			GAME_ENGINE->IsKeyboardKeyPressed('S') ||
			GAME_ENGINE->IsKeyboardKeyPressed('Z') ||
			GAME_ENGINE->IsKeyboardKeyPressed('X') ||
			GAME_ENGINE->IsKeyboardKeyPressed(VK_SPACE))
		{
			m_ShowingMessage = false;
			// NOTE: Return here so that the input isn't registered as movement input
			return;
		}
	}
	else if (GAME_ENGINE->IsKeyboardKeyPressed(VK_SPACE))
	{
		TogglePaused();
		SoundManager::PlaySoundEffect(SoundManager::SOUND::GAME_PAUSE);
	}
	if (m_Paused || m_ShowingMessage) return;

	m_SecondsElapsed += (deltaTime);
	
	if (m_ItemToBeRemovedPtr != nullptr)
	{
		// The player collided with a coin during begin-contact, we need to remove it now
		m_LevelDataPtr->RemoveItem(m_ItemToBeRemovedPtr);
		m_ItemToBeRemovedPtr = nullptr;
	}

	m_PlayerPtr->Tick(deltaTime);

	m_ParticleManagerPtr->Tick(deltaTime);

	m_LevelDataPtr->TickItemsAndEnemies(deltaTime, this);
}

void Level::AddItem(Item* newItemPtr)
{
	m_LevelDataPtr->AddItem(newItemPtr);
}

void Level::RemoveItem(Item* itemPtr)
{
	m_LevelDataPtr->RemoveItem(itemPtr);
}

void Level::AddEnemy(Enemy* newEnemyPtr)
{
	m_LevelDataPtr->AddEnemy(newEnemyPtr);
}

void Level::RemoveEnemy(Enemy* enemyPtr)
{
	m_LevelDataPtr->RemoveEnemy(enemyPtr);
}

void Level::Paint()
{
#if 0
	DEBUGPaintZoomedOut();
	return;
#endif

	MATRIX3X2 matPreviousView = GAME_ENGINE->GetViewMatrix();

	MATRIX3X2 matCameraView = m_CameraPtr->GetViewMatrix(m_PlayerPtr, this);
	MATRIX3X2 matTotalView = matCameraView *  Game::matIdentity;
	GAME_ENGINE->SetViewMatrix(matTotalView);

	int bgWidth = SpriteSheetManager::levelOneBackgroundPtr->GetWidth();
	double cameraX = m_CameraPtr->GetOffset(m_PlayerPtr, this).x;
	double parallax = (1.0 - 0.50); // 50% of the speed of the camera
	int xo = int(cameraX * parallax);
	xo += (int(cameraX - xo) / bgWidth) * bgWidth;

	GAME_ENGINE->DrawBitmap(SpriteSheetManager::levelOneBackgroundPtr, DOUBLE2(xo, 0));
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::levelOneBackgroundPtr, DOUBLE2(xo + bgWidth, 0));

	GAME_ENGINE->DrawBitmap(SpriteSheetManager::levelOneForegroundPtr);

	m_LevelDataPtr->PaintItemsAndEnemies();

	m_PlayerPtr->Paint();
	
	m_ParticleManagerPtr->Paint();

	GAME_ENGINE->SetViewMatrix(Game::matIdentity);

	PaintHUD();

	if (m_Paused && m_PlayerPtr->GetAnimationState() != Player::ANIMATION_STATE::DYING)
	{
		GAME_ENGINE->SetColor(COLOR(255, 255, 255));
		GAME_ENGINE->DrawRect(0, 0, Game::WIDTH, Game::HEIGHT, 6);
	}

#if 0
	m_Camera->DEBUGPaint();
#endif

#if 1
	GAME_ENGINE->SetColor(COLOR(0, 0, 0));
	GAME_ENGINE->SetFont(Game::Font12Ptr);
	GAME_ENGINE->DrawString(String("pos: ") + m_PlayerPtr->GetPosition().ToString(), 10, 193);
	GAME_ENGINE->DrawString(String("vel: ") + m_PlayerPtr->GetLinearVelocity().ToString(), 10, 203);
	GAME_ENGINE->DrawString(String("onGround: ") + String(m_PlayerPtr->IsOnGround() ? "true" : "false"), 10, 213);
#endif

	GAME_ENGINE->SetViewMatrix(matTotalView);
}

// TODO: Make this scalable
void Level::PaintHUD()
{
	int playerLives = m_PlayerPtr->GetLives();
	int playerDragonCoins = m_PlayerPtr->GetDragonCoinsCollected();
	int playerStars = m_PlayerPtr->GetStarsCollected();
	int playerCoins = m_PlayerPtr->GetCoinsCollected();
	int playerScore = m_PlayerPtr->GetScore();
	Item::TYPE playerExtraItemType = m_PlayerPtr->GetExtraItemType();

	// NOTE: 1 second in SMW is 2/3 of a real life second!
	int timeRemaining = m_TotalTime - (int(m_SecondsElapsed * 1.5)); 

	int x = 15;
	int y = 15;
	RECT2 srcRect;

	// LATER: Add luigi here when he's playing
	// MARIO
	srcRect = RECT2(1, 1, 41, 9);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::hudPtr, x, y, srcRect);
	
	// X
	x += 9;
	y += 8;
	srcRect = RECT2(10, 61, 10 + 7, 61 + 7);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::hudPtr, x, y, srcRect);

	// LIVES
	x += 15;
	srcRect = GetSmallSingleNumberSrcRect(playerLives, false);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::hudPtr, x, y, srcRect);

	// DRAGON COINS
	x += 24;
	y = 15;
	for (int i = 0; i < playerDragonCoins; ++i)
	{
		srcRect = RECT2(1, 60, 1 + 8, 60 + 8);
		GAME_ENGINE->DrawBitmap(SpriteSheetManager::hudPtr, x, y, srcRect);
		x += 8;
	}

	// RED STAR
	y += 8;
	x = 70;
	srcRect = RECT2(19, 60, 19 + 8, 60 + 8);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::hudPtr, x, y, srcRect);

	// X
	x += 10;
	y += 2;
	srcRect = RECT2(10, 61, 10 + 7, 61 + 7);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::hudPtr, x, y, srcRect);

	// NUMBER OF STARS
	x += 24;
	y -= 7;
	PaintSeveralDigitLargeNumber(x, y, playerStars);

	// ITEM BOX
	x += 10;
	y -= 7;
	srcRect = RECT2(36, 52, 36 + 28, 52 + 28);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::hudPtr, x, y, srcRect);

	// TIME
	x += 37;
	y = 15;
	srcRect = RECT2(1, 52, 1 + 24, 52 + 7);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::hudPtr, x, y, srcRect);

	// TIME VALUE
	y += 9;
	x += 16;
	PaintSeveralDigitNumber(x, y, timeRemaining, true);

	// COIN LABEL
	x += 33;
	y = 15;
	srcRect = RECT2(1, 60, 1 + 16, 60 + 8);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::hudPtr, x, y, srcRect);

	// COINS
	x += 30;
	PaintSeveralDigitNumber(x, y, playerCoins, false);

	// SCORE
	y += 8;
	PaintSeveralDigitNumber(x, y, playerScore, false);
}

// TODO: Just set a world matrix to print these instead of passing numbers
// NOTE: The x coordinate specifies the placement of the right-most digit
void PaintSeveralDigitNumber(int x, int y, int number, bool yellow)
{
	number = abs(number);

	do {
		int digit = number % 10;
		RECT2 srcRect = GetSmallSingleNumberSrcRect(digit, yellow);
		GAME_ENGINE->DrawBitmap(SpriteSheetManager::hudPtr, x, y, srcRect);

		x -= 8;
		number /= 10;
	} while (number > 0);
}

// TODO: Move to a more global class (custom math class?)
unsigned int Level::GetNumberOfDigits(unsigned int i)
{
	return i > 0 ? (int)log10((double)i) + 1 : 1;
}

// NOTE: If yellow is true, this returns the rect for a yellow number, otherwise for a white number
// TODO: Fix slight issue with bitmap (6 appears to close to the left of the number next to it)
RECT2 GetSmallSingleNumberSrcRect(int number, bool yellow)
{
	assert(number >= 0 && number <= 9);

	RECT2 result;

	int numberWidth = 8;
	int numberHeight = 7;
	int xo = 0 + numberWidth * number;
	int yo = 34;

	if (yellow) yo += 10;

	result = RECT2(xo, yo, xo + numberWidth, yo + numberHeight);

	return result;
}

void PaintSeveralDigitLargeNumber(int x, int y, int number)
{
	number = abs(number);

	do {
		int digit = number % 10;
		RECT2 srcRect = GetLargeSingleNumberSrcRect(digit);
		GAME_ENGINE->DrawBitmap(SpriteSheetManager::hudPtr, x, y, srcRect);

		x -= 8;
		number /= 10;
	} while (number > 0);
}

RECT2 GetLargeSingleNumberSrcRect(int number)
{
	assert(number >= 0 && number <= 9);

	RECT2 result;

	int numberWidth = 8;
	int numberHeight = 14;
	int xo = 0 + numberWidth * number;
	int yo = 19;

	result = RECT2(xo, yo, xo + numberWidth, yo + numberHeight);

	return result;
}

 void Level::DEBUGPaintZoomedOut()
{
	MATRIX3X2 matCameraView = m_CameraPtr->GetViewMatrix(m_PlayerPtr, this);
	MATRIX3X2 matZoom = MATRIX3X2::CreateScalingMatrix(0.25);
	MATRIX3X2 matCenterTranslate = MATRIX3X2::CreateTranslationMatrix(150, 160);
	GAME_ENGINE->SetViewMatrix(matCameraView * matZoom * matCenterTranslate);

	int bgWidth = SpriteSheetManager::levelOneBackgroundPtr->GetWidth();
	double cameraX = -matCameraView.orig.x;

	double parallax = (1.0 - 0.65); // 65% of the speed of the camera
	int xo = int(cameraX*parallax);
	xo += (int(cameraX - xo) / bgWidth) * bgWidth;

	GAME_ENGINE->DrawBitmap(SpriteSheetManager::levelOneBackgroundPtr, DOUBLE2(xo, 0));
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::levelOneBackgroundPtr, DOUBLE2(xo + bgWidth, 0));

	int nearestBoundary = int(cameraX - (int(cameraX) % bgWidth));
	GAME_ENGINE->SetColor(COLOR(200, 20, 20));
	GAME_ENGINE->DrawLine(nearestBoundary, 0, nearestBoundary, 5000, 5);
	GAME_ENGINE->DrawLine(nearestBoundary + bgWidth, 0, nearestBoundary + bgWidth, 5000, 5);

	GAME_ENGINE->SetColor(COLOR(20, 20, 20));
	GAME_ENGINE->DrawLine(xo + bgWidth, 0, xo + bgWidth, 5000, 5);

	GAME_ENGINE->DrawBitmap(SpriteSheetManager::levelOneForegroundPtr);

	m_PlayerPtr->Paint();

	// Draw camera outline
	GAME_ENGINE->SetWorldMatrix(matCameraView.Inverse());
	GAME_ENGINE->SetColor(COLOR(20, 20, 200));
	GAME_ENGINE->DrawRect(5, 5, Game::WIDTH - 5, Game::HEIGHT - 5, 5);
	GAME_ENGINE->SetWorldMatrix(Game::matIdentity);

	GAME_ENGINE->SetViewMatrix(Game::matIdentity);

	GAME_ENGINE->SetColor(COLOR(0, 0, 0));
	GAME_ENGINE->DrawString(String("Player pos: ") + m_PlayerPtr->GetPosition().ToString(), 10, 10);

	GAME_ENGINE->SetViewMatrix(matCameraView);
}

bool Level::IsPlayerOnGround()
{
	return m_IsPlayerOnGround;
}

void Level::PreSolve(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr, bool & enableContactRef)
{
	DOUBLE2 playerFeet(m_PlayerPtr->GetPosition().x, m_PlayerPtr->GetPosition().y + m_PlayerPtr->GetHeight() / 2);

	switch (actOtherPtr->GetUserData())
	{
	case int(ActorId::PLAYER):
	{
		bool playerIsRising = actOtherPtr->GetLinearVelocity().y < -0.001;

		switch (actThisPtr->GetUserData())
		{
		case int(ActorId::PLATFORM):
		{
			DOUBLE2 platformPos = actThisPtr->GetPosition();
			double halfPlayerWidth = m_PlayerPtr->GetWidth() / 2;
			double halfPlatformWidth = ((Platform*)actThisPtr->GetUserPointer())->GetWidth() / 2.0;
			double halfPlatformHeight = ((Platform*)actThisPtr->GetUserPointer())->GetHeight() / 2.0;

			bool playerToLeftOfPlatform = playerFeet.x + halfPlayerWidth < platformPos.x - halfPlatformWidth;
			bool playerToRightOfPlatform = playerFeet.x + halfPlayerWidth < platformPos.x - halfPlatformWidth;
			bool playerFeetBelowPlatformBottom = playerFeet.y > platformPos.y + halfPlatformHeight;

			bool playerFeetAreBelowPlatformTop = playerFeet.y > platformPos.y - halfPlatformHeight;
			bool playerCenterToLeftOfPlatform = playerFeet.x < platformPos.x - halfPlatformWidth;
			bool playerCenterToRightOfPlatform = playerFeet.x > platformPos.x + halfPlatformWidth;

			if ((playerFeetAreBelowPlatformTop && (playerCenterToLeftOfPlatform || playerCenterToRightOfPlatform)) ||
				playerToLeftOfPlatform || playerToRightOfPlatform || playerFeetBelowPlatformBottom)
			{
				enableContactRef = false;
			}
		} break;
		case int(ActorId::LEVEL):
		case int(ActorId::PIPE):
		{
			if (playerIsRising)
			{
				actOtherPtr->SetLinearVelocity(DOUBLE2(0, actOtherPtr->GetLinearVelocity().y));
				enableContactRef = false;
			}
		} break;
		case int(ActorId::ITEM):
		{
			Item* itemPtr = (Item*)actThisPtr->GetUserPointer();
			switch (itemPtr->GetType())
			{
			case Item::TYPE::P_SWITCH:
			{
				// LATER: Temporarily change all coins into blocks and vice versa
			} break;
			case Item::TYPE::PRIZE_BLOCK:
			case Item::TYPE::MESSAGE_BLOCK:
			case Item::TYPE::EXCLAMATION_MARK_BLOCK:
			case Item::TYPE::ROTATING_BLOCK:
			{
				if (itemPtr->GetType() == Item::TYPE::ROTATING_BLOCK)
				{
					if (((RotatingBlock*)itemPtr)->IsRotating())
					{
						enableContactRef = false;
						return;
					}
				}

				DOUBLE2 blockCenterPos = actThisPtr->GetPosition();
				bool playerIsBesideBlock = (playerFeet.x + m_PlayerPtr->GetWidth() / 2 < blockCenterPos.x - Block::WIDTH / 2) ||
										   (playerFeet.x - m_PlayerPtr->GetWidth() / 2 > blockCenterPos.x + Block::WIDTH / 2);
				if (playerIsBesideBlock)
				{
					enableContactRef = false;

					bool playersFeetAreBelowTopOfBlock = (playerFeet.y >= blockCenterPos.y - Block::HEIGHT / 2);
					if (playersFeetAreBelowTopOfBlock)
					{
						m_PlayerPtr->SetLinearVelocity(DOUBLE2(0, m_PlayerPtr->GetLinearVelocity().y));
					}
				}
			} break;
			}
		} break;
		}
	} break;
	case int(ActorId::ITEM):
	{
		// FIXME: IMPORTANT: TODO: Find out how tf to know when to use 'actThis'
		// vs. 'actOther', it sure seems like black magic
		Item* otherItemPtr = (Item*)actOtherPtr->GetUserPointer();
		switch (otherItemPtr->GetType())
		{
		case Item::TYPE::P_SWITCH:
		{
			if (actThisPtr->GetUserData() == int(ActorId::ITEM))
			{
				Item* thisItem = (Item*)actThisPtr->GetUserPointer();
				if (thisItem->GetType() == Item::TYPE::ROTATING_BLOCK)
				{
					// NOTE: Only let it fall through if the rotating block is spinning
					if (((RotatingBlock*)thisItem)->IsRotating())
					{
						enableContactRef = false;
					}
				}
			}
		} break;
		}
	} break;
	}
}

void Level::BeginContact(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr)
{
	switch (actOtherPtr->GetUserData())
	{
	case int(ActorId::PLAYER):
	{
		bool playerIsRising = actOtherPtr->GetLinearVelocity().y < -0.001;
		DOUBLE2 playerFeet = DOUBLE2(actOtherPtr->GetPosition().x, actOtherPtr->GetPosition().y + ((Player*)actOtherPtr)->GetHeight() / 2);

		switch (actThisPtr->GetUserData())
		{
		case int(ActorId::PLATFORM):
		{
			// NOTE: *Add* half of the platform height to catch when the player's feet are inside the platform 
			if (playerFeet.y <= actThisPtr->GetPosition().y + Platform::HEIGHT/2)
			{
				m_IsPlayerOnGround = true;
			}
		} break;
		case int(ActorId::LEVEL):
		case int(ActorId::PIPE):
		{
			// TODO: Prevent player from jumping when running sideways into a vertical section of the level
			m_IsPlayerOnGround = true;
		} break;
		case int(ActorId::ITEM):
		{
			Item* item = (Item*)actThisPtr->GetUserPointer();
			switch (item->GetType())
			{
			case Item::TYPE::COIN:
			case Item::TYPE::DRAGON_COIN:
			case Item::TYPE::SUPER_MUSHROOM:
			case Item::TYPE::STAR:
			case Item::TYPE::FIRE_FLOWER:
			case Item::TYPE::CAPE_FEATHER:
			case Item::TYPE::POWER_BALLOON:
			case Item::TYPE::ONE_UP_MUSHROOM:
			case Item::TYPE::THREE_UP_MOON:
			{
				if (m_ItemToBeRemovedPtr != nullptr)
				{
					// The player is collecting two (or more) items this tick, just return and let the 
					// previous item be collected, we'll almost definitely be colliding with this one again next tick
					break;
				}

				((Player*)actOtherPtr->GetUserPointer())->OnItemPickup(item, this);
				m_ItemToBeRemovedPtr = item;
			} break;
			case Item::TYPE::PRIZE_BLOCK:
			case Item::TYPE::EXCLAMATION_MARK_BLOCK:
			case Item::TYPE::MESSAGE_BLOCK:
			case Item::TYPE::ROTATING_BLOCK:
			{
				bool playerCenterIsBelowBlock = m_PlayerPtr->GetPosition().y > (actThisPtr->GetPosition().y + Block::HEIGHT / 2);
				bool playerCenterIsAboveBlock = m_PlayerPtr->GetPosition().y < (actThisPtr->GetPosition().y - Block::HEIGHT / 2);

				if (playerIsRising && playerCenterIsBelowBlock)
				{
					((Block*)item)->Hit(this);

					// NOTE: This line prevents the player from slowly floating down after hitting a block
					m_PlayerPtr->SetLinearVelocity(DOUBLE2(m_PlayerPtr->GetLinearVelocity().x, 0.0));
				}
				else if (playerCenterIsAboveBlock)
				{
					m_IsPlayerOnGround = true;
				}
			} break;
			}
		} break;
		case int(ActorId::ENEMY):
		{
			Enemy* enemyPtr = (Enemy*)actThisPtr->GetUserPointer();
			switch (enemyPtr->GetType())
			{
			case Enemy::TYPE::KOOPA_TROOPA:
			{
				if (playerFeet.y < enemyPtr->GetPosition().y)
				{
					if (m_PlayerPtr->GetAnimationState() == Player::ANIMATION_STATE::SPIN_JUMPING)
					{
						((KoopaTroopa*)enemyPtr)->Die();
					}
					else
					{
						((KoopaTroopa*)enemyPtr)->Hit();
					}
				}
				else
				{
					if (((KoopaTroopa*)enemyPtr)->GetAnimationState() == KoopaTroopa::ANIMATION_STATE::SHELLESS)
					{
						((KoopaTroopa*)enemyPtr)->Die();
					}
					else
					{
						m_PlayerPtr->Die();
					}
				}
			} break;
			case Enemy::TYPE::CHARGIN_CHUCK:
			{

			} break;
			case Enemy::TYPE::MONTY_MOLE:
			{

			} break;
			case Enemy::TYPE::PIRHANA_PLANT:
			{

			} break;
			}
		} break;
		}
	} break;
	case int(ActorId::ITEM):
	{
		if (actThisPtr->GetUserData() == int(ActorId::LEVEL))
		{
			switch (((Item*)actOtherPtr->GetUserPointer())->GetType())
			{
			case Item::TYPE::SUPER_MUSHROOM:
			{
				// TODO: FIXME: For some reason svg bodies don't seem to ever return true on IsPointInActor()
				DOUBLE2 point(DOUBLE2(actOtherPtr->GetPosition().x + 32, actOtherPtr->GetPosition().y));
				if (actThisPtr->IsPointInActor(point))
				{
					actOtherPtr->SetLinearVelocity(DOUBLE2(
						-actOtherPtr->GetLinearVelocity().x,
						actOtherPtr->GetLinearVelocity().y));
				}
			} break;
			}
		}
	} break;
	}
}

void Level::EndContact(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr)
{
	bool isOverlapping = actThisPtr->IsOverlapping(actOtherPtr);
	if (actOtherPtr->GetUserData() == int(ActorId::PLAYER))
	{
		//if (abs(actOtherPtr->GetLinearVelocity().y) > 0.01)
		if (!isOverlapping)
		{
			m_IsPlayerOnGround = false;
		}
	}
}

// NOTE: This seems like a place where Box2D's filter system would be handy to use
// however I don't think raycasting listens to that, so we'll just implement our own
// bitfields
bool Level::Raycast(DOUBLE2 point1, DOUBLE2 point2, int collisionBits, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef)
{
	if (collisionBits & COLLIDE_WITH_LEVEL)
	{
		if (m_ActLevelPtr->Raycast(point1, point2, intersectionRef, normalRef, fractionRef))
		{
			return true;
		}

		std::vector<Pipe*> pipesPtrArr = m_LevelDataPtr->GetPipes();
		for (size_t i = 0; i < pipesPtrArr.size(); ++i)
		{
			if (pipesPtrArr[i]->Raycast(point1, point2, intersectionRef, normalRef, fractionRef))
			{
				return true;
			}
		}

		std::vector<Platform*> platformsPtrArr = m_LevelDataPtr->GetPlatforms();
		for (size_t i = 0; i < platformsPtrArr.size(); ++i)
		{
			if (platformsPtrArr[i]->Raycast(point1, point2, intersectionRef, normalRef, fractionRef))
			{
				return true;
			}
		}
	}

	if (collisionBits & COLLIDE_WITH_ENEMIES)
	{
		std::vector<Enemy*> enemiesPtrArr = m_LevelDataPtr->GetEnemies();
		for (size_t i = 0; i < enemiesPtrArr.size(); ++i)
		{
			if (enemiesPtrArr[i] != nullptr)
			{
				if (enemiesPtrArr[i]->Raycast(point1, point2, intersectionRef, normalRef, fractionRef))
				{
					return true;
				}
			}
		}
	}

	if (collisionBits & COLLIDE_WITH_PLAYER)
	{
		if (m_PlayerPtr->Raycast(point1, point2, intersectionRef, normalRef, fractionRef))
		{
			return true;
		}
	}

	return false;
}

void Level::AddParticle(Particle* particlePtr)
{
	m_ParticleManagerPtr->AddParticle(particlePtr);
}

DOUBLE2 Level::GetCameraOffset()
{
	return m_CameraPtr->GetOffset(m_PlayerPtr, this);
}

Player* Level::GetPlayer()
{
	return m_PlayerPtr;
}

void Level::GiveItemToPlayer(Item* itemPtr)
{
	m_PlayerPtr->OnItemPickup(itemPtr, this);
}

void Level::TogglePaused()
{
	SetPaused(!m_Paused);
}

void Level::SetPaused(bool paused)
{
	m_Paused = paused;

	m_PlayerPtr->SetPaused(m_Paused);
	m_LevelDataPtr->SetItemsAndEnemiesPaused(m_Paused);

	SoundManager::SetAllSongsPaused(m_Paused);
}

void Level::SetShowingMessage(bool showingMessage)
{
	m_ShowingMessage = showingMessage;
}

bool Level::IsShowingMessage()
{
	return m_ShowingMessage;
}

double Level::GetWidth()
{
	return m_Width;
}

double Level::GetHeight()
{
	return m_Height;
}
