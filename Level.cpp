#include "stdafx.h"

#include "Level.h"
#include "Game.h"
#include "Camera.h"
#include "LevelData.h"
#include "SpriteSheetManager.h"
#include "SoundManager.h"

#include "Platform.h"
#include "Pipe.h"
#include "Yoshi.h"

#include "Block.h"
#include "RotatingBlock.h"
#include "Particle.h"
#include "ParticleManager.h"
#include "Player.h"
#include "KoopaTroopa.h"
#include "MontyMole.h"
#include "SuperMushroom.h"
#include "KoopaShell.h"
#include "MidwayGate.h"
#include "GoalGate.h"

#define GAME_ENGINE (GameEngine::GetSingleton())

// NOTE: Level::Reset *must* be called upon game initialization!
Level::Level()
{
	m_PlayerPtr = new Player(this);

	m_ActLevelPtr = new PhysicsActor(DOUBLE2(0, 0), 0, BodyType::STATIC);
	m_ActLevelPtr->AddSVGFixture(String("Resources/levels/01/Level01hit.svg"), 0.0);
	m_ActLevelPtr->AddContactListener(this);
	m_ActLevelPtr->SetUserData(int(ActorId::LEVEL));

	SpriteSheetManager::levelOneForegroundPtr->SetTransparencyColor(COLOR(255, 0, 255));

	m_Width = SpriteSheetManager::levelOneForegroundPtr->GetWidth();
	m_Height = SpriteSheetManager::levelOneForegroundPtr->GetHeight();

	m_CameraPtr = new Camera(Game::WIDTH, Game::HEIGHT, this);

	m_ParticleManagerPtr = new ParticleManager();
}

Level::~Level()
{
	delete m_ActLevelPtr;
	delete m_PlayerPtr;
	delete m_CameraPtr;
	delete m_ParticleManagerPtr;
	delete m_YoshiPtr;
}

void Level::Reset()
{
	m_PlayerPtr->Reset();
	m_CameraPtr->Reset();

	m_Paused = false;
	m_ShowingMessage = false;

	m_TotalTime = 400; // This changes for every level, TODO: Put this info in the level save
	m_SecondsElapsed = 0.0;
	m_TimeRemaining = m_TotalTime;

	m_IsCheckpointCleared = false;

	m_GamePausedTimer; // this timer's value is set by other classes (in Level::SetPausedTimer)

	m_AllDragonCoinsCollected = false;

	m_LevelDataPtr->GenerateLevelData(1, this);
	ReadLevelData(1);

	delete m_YoshiPtr;
	m_YoshiPtr = nullptr;

	m_IsShowingEndScreen = false;
	m_FramesShowingEndScreen = 0;

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
	if (m_GamePausedTimer.Tick() && m_GamePausedTimer.IsComplete())
	{
		SetPaused(false);
	}

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
	if (m_Paused && 
		(m_PlayerPtr->IsDead() || m_PlayerPtr->IsTransitioningPowerups()))
	{
		// NOTE: The player needs to still be ticked so they can animate
		m_PlayerPtr->Tick(deltaTime);
		return;
	}
	else if (m_Paused || m_ShowingMessage) return;

	if (m_IsShowingEndScreen)
	{
		return;
	}

	m_SecondsElapsed += (deltaTime);

	for (size_t i = 0; i < m_ItemsToBeRemovedPtrArr.size(); ++i)
	{
		if (m_ItemsToBeRemovedPtrArr[i] != nullptr)
		{
			m_LevelDataPtr->RemoveItem(m_ItemsToBeRemovedPtrArr[i]);
			m_ItemsToBeRemovedPtrArr[i] = nullptr;
		}
	}

	if (m_PlayerPtr->GetExtraItemType() != Item::TYPE::NONE &&
		m_Paused && GAME_ENGINE->IsKeyboardKeyPressed(VK_RETURN))
	{
		DOUBLE2 cameraOffset = GetCameraOffset();
		double xPos = cameraOffset.x + Game::WIDTH / 2;
		double yPos = cameraOffset.y + 25;

		m_PlayerPtr->ReleaseExtraItem(DOUBLE2(xPos, yPos));
	}

	if (m_YoshiPtr != nullptr)
	{
		m_YoshiPtr->Tick(deltaTime);
	}

	m_PlayerPtr->Tick(deltaTime);

	m_ParticleManagerPtr->Tick(deltaTime);

	m_LevelDataPtr->TickItemsAndEnemies(deltaTime, this);
}

void Level::AddYoshi(Yoshi* yoshiPtr)
{
	if (m_YoshiPtr != nullptr) 
	{
		OutputDebugString(String("ERROR: More than one yoshi was added to the level!\n"));
		delete m_YoshiPtr;
	}

	m_YoshiPtr = yoshiPtr;
	yoshiPtr->AddContactListener(this);
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
	MATRIX3X2 matPreviousView = GAME_ENGINE->GetViewMatrix();

	MATRIX3X2 matCameraView = m_CameraPtr->GetViewMatrix(m_PlayerPtr, this);
	MATRIX3X2 matTotalView = matCameraView *  Game::matIdentity;
	GAME_ENGINE->SetViewMatrix(matTotalView);

	int bgWidth = SpriteSheetManager::levelOneBackgroundPtr->GetWidth();
	double cameraX = m_CameraPtr->GetOffset(m_PlayerPtr, this).x;
	double parallax = (1.0 - 0.50); // 50% of the speed of the camera
	int xo = int(cameraX * parallax);
	xo += (int(cameraX - xo) / bgWidth) * bgWidth;

	GAME_ENGINE->DrawBitmap(SpriteSheetManager::levelOneBackgroundPtr, DOUBLE2(xo, -32));
	if (Game::WIDTH - (cameraX - xo) < 0)
	{
		GAME_ENGINE->DrawBitmap(SpriteSheetManager::levelOneBackgroundPtr, DOUBLE2(xo + bgWidth, -32));
	}

	GAME_ENGINE->DrawBitmap(SpriteSheetManager::levelOneForegroundPtr);

	m_LevelDataPtr->PaintItemsAndEnemies();

	if (m_YoshiPtr != nullptr)
	{
		m_YoshiPtr->Paint();
	}
	m_PlayerPtr->Paint();

	m_LevelDataPtr->PaintItemsForeground();

	m_ParticleManagerPtr->Paint();

	GAME_ENGINE->SetViewMatrix(Game::matIdentity);

	int fadeTransition = 1600; // after this many frames, the transition will occur
	int drumrollStart = fadeTransition + 1600;
	int fadeFromBlackTransition = 3550 + fadeTransition;
	int backToClearTransition = fadeTransition + fadeFromBlackTransition;
	int resetGameTransition = backToClearTransition + 128;
	if (m_IsShowingEndScreen)
	{
		++m_FramesShowingEndScreen;

		if (m_FramesShowingEndScreen < fadeTransition)
		{
			GAME_ENGINE->SetColor(COLOR(0, 0, 0, int((double(m_FramesShowingEndScreen) / double(fadeTransition)) * 255)));
		}
		else if (m_FramesShowingEndScreen < fadeFromBlackTransition)
		{
			if (m_FramesShowingEndScreen == drumrollStart)
			{
				SoundManager::PlaySoundEffect(SoundManager::SOUND::DRUMROLL);
			}
			GAME_ENGINE->SetColor(COLOR(0, 0, 0));
		}
		else if (m_FramesShowingEndScreen < backToClearTransition)
		{
			int alpha = int(255 - (double(m_FramesShowingEndScreen - fadeFromBlackTransition) / double(fadeTransition)) * 255);
			GAME_ENGINE->SetColor(COLOR(0, 0, 0, alpha));
		}
		else
		{
			if (m_FramesShowingEndScreen == backToClearTransition)
			{
				SoundManager::PlaySoundEffect(SoundManager::SOUND::OUTRO_CIRCLE_TRANSITION);
			}
			else if (m_FramesShowingEndScreen >= resetGameTransition)
			{
				Reset();
				return;
			}
			GAME_ENGINE->SetColor(COLOR(0, 0, 0, 0));
		}


		GAME_ENGINE->FillRect(0, 0, Game::WIDTH, Game::HEIGHT);
	}


#if SMW_DEBUG_ZOOM_OUT
	GAME_ENGINE->SetColor(COLOR(250, 10, 10));
	GAME_ENGINE->DrawRect(0, 0, Game::WIDTH, Game::HEIGHT, 2.5);
#endif

	PaintHUD();

#if SMW_DISPLAY_CAMERA_DEBUG_INFO
	m_CameraPtr->DEBUGPaint();
#endif

#if SMW_DISPLAY_GENERAL_DEBUG_INFO
	GAME_ENGINE->SetColor(COLOR(0, 0, 0));
	GAME_ENGINE->SetFont(Game::Font9Ptr);
	int yo = 197;
	int dy = 9;
	GAME_ENGINE->DrawString(String("pos: ") + m_PlayerPtr->GetPosition().ToString(), 10, yo); yo += dy;
	GAME_ENGINE->DrawString(String("vel: ") + m_PlayerPtr->GetLinearVelocity().ToString(), 10, yo); yo += dy;
	GAME_ENGINE->DrawString(String("onGround: ") + String(m_PlayerPtr->IsOnGround() ? "true" : "false"), 10, yo); yo -= dy * 2;

	if (SoundManager::IsMuted())
	{
		GAME_ENGINE->DrawString(String("m"), 245, 198);
	}
#endif


	GAME_ENGINE->SetViewMatrix(matTotalView);
}

void Level::PaintHUD()
{
	int playerLives = m_PlayerPtr->GetLives();
	int playerDragonCoins = m_PlayerPtr->GetDragonCoinsCollected();
	int playerStars = m_PlayerPtr->GetStarsCollected();
	int playerCoins = m_PlayerPtr->GetCoinsCollected();
	int playerScore = m_PlayerPtr->GetScore();
	Item::TYPE playerExtraItemType = m_PlayerPtr->GetExtraItemType();

	// NOTE: 1 second in SMW is 2/3 of a real life second!
	m_TimeRemaining = m_TotalTime - (int(m_SecondsElapsed * 1.5)); 

	int x = 15;
	int y = 15;
	RECT2 srcRect;

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
	PaintSeveralDigitNumber(x, y, m_TimeRemaining, true);

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

bool Level::ActorCanPassThroughPlatform(PhysicsActor *actPlatformPtr, DOUBLE2& actorPosRef, double actorWidth, double actorHeight)
{
	DOUBLE2 actorFeet = DOUBLE2(actorPosRef.x, actorPosRef.y + actorHeight / 2);

	double halfPlatformWidth = ((Platform*)actPlatformPtr->GetUserPointer())->GetWidth() / 2.0;
	double halfPlatformHeight = ((Platform*)actPlatformPtr->GetUserPointer())->GetHeight() / 2.0;

	DOUBLE2 platformPos = actPlatformPtr->GetPosition();
	bool actorToLeftOfPlatform = actorFeet.x + actorWidth / 2 < platformPos.x - halfPlatformWidth;
	bool actorToRightOfPlatform = actorFeet.x + actorWidth / 2 < platformPos.x - halfPlatformWidth;
	bool actorFeetBelowPlatformBottom = actorFeet.y > platformPos.y + halfPlatformHeight;

	bool actorFeetAreBelowPlatformTop = actorFeet.y > platformPos.y - halfPlatformHeight;
	bool actorCenterToLeftOfPlatform = actorFeet.x < platformPos.x - halfPlatformWidth;
	bool actorCenterToRightOfPlatform = actorFeet.x > platformPos.x + halfPlatformWidth;

	return ((actorFeetAreBelowPlatformTop && (actorCenterToLeftOfPlatform || actorCenterToRightOfPlatform)) ||
		actorToLeftOfPlatform || 
		actorToRightOfPlatform || 
		actorFeetBelowPlatformBottom);
}

void Level::PreSolve(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr, bool & enableContactRef)
{
	DOUBLE2 playerFeet(m_PlayerPtr->GetPosition().x, m_PlayerPtr->GetPosition().y + m_PlayerPtr->GetHeight() / 2);

	switch (actOtherPtr->GetUserData())
	{
	case int(ActorId::PLAYER):
	{
		assert(m_PlayerPtr == actOtherPtr->GetUserPointer());

		if (m_PlayerPtr->IsDead())
		{
			break;
		}

		bool playerIsRising = actOtherPtr->GetLinearVelocity().y < -0.001;

		switch (actThisPtr->GetUserData())
		{
		case int(ActorId::PLATFORM):
		{
			if (ActorCanPassThroughPlatform(actThisPtr, m_PlayerPtr->GetPosition(), m_PlayerPtr->GetWidth(), m_PlayerPtr->GetHeight()))
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
				// TODO: Temporarily change all coins into blocks and vice versa
			} break;
			case Item::TYPE::PRIZE_BLOCK:
			case Item::TYPE::MESSAGE_BLOCK:
			case Item::TYPE::EXCLAMATION_MARK_BLOCK:
			case Item::TYPE::ROTATING_BLOCK:
			case Item::TYPE::CLOUD_BLOCK:
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
			case Item::TYPE::KOOPA_SHELL:
			{
				enableContactRef = false;
			} break;
			}
		} break;
		case int(ActorId::YOSHI):
		{
			if (m_PlayerPtr->IsRidingYoshi()) enableContactRef = false;
		} break;
		case int(ActorId::ENEMY):
		{
			if (m_PlayerPtr->IsInvincible()) enableContactRef = false;
		} break;
		}
	} break;
	case int(ActorId::ENEMY):
	{
		switch (actThisPtr->GetUserData())
		{
		case int(ActorId::PLATFORM):
		{
			double enemyWidth = ((Enemy*)actOtherPtr->GetUserPointer())->GetWidth();
			double enemyHeight = ((Enemy*)actOtherPtr->GetUserPointer())->GetHeight();
			if (ActorCanPassThroughPlatform(actThisPtr, actOtherPtr->GetPosition(), enemyWidth, enemyHeight))
			{
				enableContactRef = false;
			}
		} break;
		case int(ActorId::ENEMY):
		{
			Enemy* thisEnemyPtr = (Enemy*)actThisPtr->GetUserPointer();
			Enemy* otherEnemyPtr = (Enemy*)actOtherPtr->GetUserPointer();
			if (otherEnemyPtr->GetType() == Enemy::TYPE::MONTY_MOLE && 
				thisEnemyPtr->GetType() == otherEnemyPtr->GetType())
			{
				enableContactRef = false;
			}
		} break;
		}
	} break;
	case int(ActorId::ITEM):
	{
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
		case Item::TYPE::SUPER_MUSHROOM:
		{
			if (((SuperMushroom*)otherItemPtr)->IsAnimating())
			{
				enableContactRef = false;
			}
		} break;
		case Item::TYPE::KOOPA_SHELL:
		{
			KoopaShell* koopaShellPtr =(KoopaShell*)otherItemPtr;
			if (actThisPtr->GetUserData() == int(ActorId::ITEM))
			{
				Item* thisItemPtr = (Item*)actThisPtr->GetUserPointer();
				if (thisItemPtr->IsBlock())
				{
					DOUBLE2 koopaShellVel = actOtherPtr->GetLinearVelocity();
					koopaShellPtr->SetLinearVelocity(DOUBLE2(-koopaShellVel.x, koopaShellVel.y));
				}
			}
		} break;
		}
	} break;
	case int(ActorId::PLATFORM):
	{
		if (actThisPtr->GetUserData() == int(ActorId::ITEM))
		{
			Item* itemPtr = (Item*)actThisPtr->GetUserPointer();
			if (itemPtr->GetType() == Item::TYPE::KOOPA_SHELL)
			{
				if (ActorCanPassThroughPlatform(actOtherPtr, actThisPtr->GetPosition(), Item::TILE_SIZE, Item::TILE_SIZE))
				{
					enableContactRef = false;
				}
			}
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
		if (m_PlayerPtr->IsDead())
		{
			break;
		}

		bool playerIsRising = actOtherPtr->GetLinearVelocity().y < -0.001;
		DOUBLE2 playerFeet = DOUBLE2(actOtherPtr->GetPosition().x, actOtherPtr->GetPosition().y + ((Player*)actOtherPtr)->GetHeight() / 2);

		switch (actThisPtr->GetUserData())
		{
		case int(ActorId::ITEM):
		{
			Item* itemPtr = (Item*)actThisPtr->GetUserPointer();
			switch (itemPtr->GetType())
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
				((Player*)actOtherPtr->GetUserPointer())->OnItemPickup(itemPtr, this);
				m_ItemsToBeRemovedPtrArr.push_back(itemPtr);
			} break;
			case Item::TYPE::PRIZE_BLOCK:
			case Item::TYPE::EXCLAMATION_MARK_BLOCK:
			case Item::TYPE::MESSAGE_BLOCK:
			case Item::TYPE::ROTATING_BLOCK:
			case Item::TYPE::CLOUD_BLOCK:
			{
				bool playerCenterIsBelowBlock = m_PlayerPtr->GetPosition().y > (actThisPtr->GetPosition().y + Block::HEIGHT / 2);
				bool playerCenterIsAboveBlock = m_PlayerPtr->GetPosition().y < (actThisPtr->GetPosition().y - Block::HEIGHT / 2);

				if (playerIsRising && playerCenterIsBelowBlock)
				{
					((Block*)itemPtr)->Hit(this);

					// NOTE: This line prevents the player from slowly floating down after hitting a block
					m_PlayerPtr->SetLinearVelocity(DOUBLE2(m_PlayerPtr->GetLinearVelocity().x, 0.0));
				}
				else if (playerCenterIsAboveBlock)
				{
				}
			} break;
			case Item::TYPE::KOOPA_SHELL:
			{
				KoopaShell* koopaShellPtr = (KoopaShell*)itemPtr;
				if (m_PlayerPtr->GetAnimationState() == Player::ANIMATION_STATE::SPIN_JUMPING)
				{
					koopaShellPtr->Stomp();
				}
				else if (koopaShellPtr->IsMoving() && m_PlayerPtr->IsInvincible() == false)
				{
					m_PlayerPtr->TakeDamage();
				}
				else if (m_PlayerPtr->IsRunning() && m_PlayerPtr->IsHoldingItem() == false)
				{
					if (koopaShellPtr->IsFalling() == false)
					{
						koopaShellPtr->SetMoving(false);
						m_PlayerPtr->AddItemToBeHeld(itemPtr);
					}
				}
				else 
				{
					m_PlayerPtr->KickShell(koopaShellPtr);
				}
			} break;
			case Item::TYPE::BEANSTALK:
			{
				m_PlayerPtr->SetOverlappingWithBeanstalk(true);
			} break;
			case Item::TYPE::MIDWAY_GATE:
			{
				MidwayGate* midwayGatePtr = (MidwayGate*)itemPtr;
				if (midwayGatePtr->IsHit() == false)
				{
					m_PlayerPtr->MidwayGatePasshrough();
					midwayGatePtr->Hit();
					m_IsCheckpointCleared = true;
				}
			} break;
			case Item::TYPE::GOAL_GATE:
			{
				GoalGate* goalGatePtr = (GoalGate*)itemPtr;
				if (goalGatePtr->IsHit() == false)
				{
					goalGatePtr->Hit();
				}
			} break;
			}
		} break;
		case int(ActorId::ENEMY):
		{
			Enemy* enemyPtr = (Enemy*)actThisPtr->GetUserPointer();
			DOUBLE2 enemyFeet = enemyPtr->GetPosition() + DOUBLE2(0, enemyPtr->GetHeight() / 2);
			switch (enemyPtr->GetType())
			{
			case Enemy::TYPE::KOOPA_TROOPA:
			{
				if (playerFeet.y < enemyPtr->GetPosition().y)
				{
					if (m_PlayerPtr->IsHoldingItem() == false &&
						m_PlayerPtr->GetAnimationState() == Player::ANIMATION_STATE::SPIN_JUMPING)
					{
						((KoopaTroopa*)enemyPtr)->StompKill();
						m_PlayerPtr->SetLinearVelocity(DOUBLE2(m_PlayerPtr->GetLinearVelocity().x, -58));
					}
					else
					{
						((KoopaTroopa*)enemyPtr)->HeadBonk();
						m_PlayerPtr->Bounce();
					}
				}
				else
				{
					// NOTE: If the koopa is shelless (aka laying helplessly and crying on the ground) then
					// all the player needs to do is touch them and they die
					// Once the shelless koopa starts walking (ANIMATION_STATE::WALKING_SHELLESS) then the player
					// needs to jump on their head, the player will die if they touch the side of a walking shelless koopa
					if (((KoopaTroopa*)enemyPtr)->GetAnimationState() == KoopaTroopa::ANIMATION_STATE::SHELLESS)
					{
						// TODO: Rename this method because not only shell hits cause this behaviour
						((KoopaTroopa*)enemyPtr)->ShellHit();
					}
					else if (m_PlayerPtr->IsInvincible() == false)
					{
						m_PlayerPtr->TakeDamage();
					}
				}
			} break;
			case Enemy::TYPE::MONTY_MOLE:
			{
				if (playerFeet.y < enemyPtr->GetPosition().y)
				{
					if (m_PlayerPtr->GetAnimationState() == Player::ANIMATION_STATE::SPIN_JUMPING)
					{
						((MontyMole*)enemyPtr)->StompKill();
						m_PlayerPtr->SetLinearVelocity(DOUBLE2(m_PlayerPtr->GetLinearVelocity().x, 0));
					}
					else
					{
						((MontyMole*)enemyPtr)->HeadBonk();
						m_PlayerPtr->Bounce();
					}
				}
				else
				{
					m_PlayerPtr->TakeDamage();
				}
			} break;
			}
		} break;
		case int(ActorId::YOSHI):
		{
			assert(m_YoshiPtr != nullptr);
			if (m_YoshiPtr->IsHatching() == false)
			{
				if (m_PlayerPtr->IsRidingYoshi() == false && 
					m_PlayerPtr->IsHoldingItem() == false &&
					m_PlayerPtr->GetLinearVelocity().y > 0)
				{
					m_PlayerPtr->RideYoshi(m_YoshiPtr);
				}
			}
		} break;
		}
	} break;
	case int(ActorId::ITEM):
	{
		Item* otherItemPtr = (Item*)actOtherPtr->GetUserPointer();
		switch (otherItemPtr->GetType())
		{
		case Item::TYPE::KOOPA_SHELL:
		{
			KoopaShell* otherKoopaShellPtr = (KoopaShell*) otherItemPtr;
			switch (actThisPtr->GetUserData())
			{
			case int(ActorId::ITEM):
			{
				Item* thisItemPtr = (Item*)actThisPtr->GetUserPointer();
				switch (thisItemPtr->GetType())
				{
				case Item::TYPE::KOOPA_SHELL:
				{
					// NOTE: If both shells are moving this check won't work, but
					// since that situation is very very unlikely this is fine
					if (!otherKoopaShellPtr->IsMoving())
					{
						otherKoopaShellPtr->ShellHit();
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
					if (m_PlayerPtr->IsHoldingItem() && m_PlayerPtr->GetHeldItemPtr() == otherKoopaShellPtr)
					{
						m_PlayerPtr->DropHeldItem();
						((KoopaTroopa*)enemyPtr)->ShellHit();
					}
					else if (otherKoopaShellPtr->IsMoving() || otherKoopaShellPtr->IsBouncing())
					{
						((KoopaTroopa*)enemyPtr)->ShellHit();
					}
				} break;
				}
			} break;
			}
		} break;
		}
	} break;
	}
}

void Level::EndContact(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr)
{
	if (actOtherPtr->GetUserData() == int(ActorId::PLAYER))
	{
		switch (actThisPtr->GetUserData())
		{
		case int(ActorId::ITEM):
		{
			Item* otherItemPtr = (Item*)actThisPtr->GetUserPointer();
			switch (otherItemPtr->GetType())
			{
			case Item::TYPE::BEANSTALK:
			{
				m_PlayerPtr->SetOverlappingWithBeanstalk(false);
			} break;
			}
		} break;
		case int(ActorId::YOSHI):
		{
			
		} break;
		}
	}
}

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


	if (collisionBits & COLLIDE_WITH_BLOCKS || collisionBits & COLLIDE_WITH_SHELLS)
	{
		std::vector<Item*> itemsPtrArr = m_LevelDataPtr->GetItems();
		for (size_t i = 0; i < itemsPtrArr.size(); ++i)
		{
			if (itemsPtrArr[i] != nullptr)
			{
				if ((collisionBits & COLLIDE_WITH_BLOCKS && itemsPtrArr[i]->IsBlock()) || 
					(collisionBits & COLLIDE_WITH_SHELLS && itemsPtrArr[i]->GetType() == Item::TYPE::KOOPA_SHELL))
				{
					if (itemsPtrArr[i]->Raycast(point1, point2, intersectionRef, normalRef, fractionRef))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

void Level::TriggerEndScreen()
{
	m_IsShowingEndScreen = true;

	SoundManager::PlaySoundEffect(SoundManager::SOUND::COURSE_CLEAR_FANFARE);
	SoundManager::SetAllSongsPaused(true);
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

	if (m_YoshiPtr != nullptr)
	{
		m_YoshiPtr->SetPaused(paused);
	}

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

bool Level::IsCheckpointCleared()
{
	return m_IsCheckpointCleared;
}

int Level::GetTimeRemaining()
{
	return m_TimeRemaining;
}

void Level::SetAllDragonCoinsCollected(bool allCollected)
{
	m_AllDragonCoinsCollected = allCollected;
}

bool Level::AllDragonCoinsCollected()
{
	return m_AllDragonCoinsCollected;
}

void Level::SetPausedTimer(int duration)
{
	m_GamePausedTimer = CountdownTimer(duration);
	m_GamePausedTimer.Start();
	SetPaused(true);
}
