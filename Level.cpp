#include "stdafx.h"

#include "Level.h"
#include "Game.h"
#include "GameState.h"
#include "Camera.h"
#include "LevelData.h"
#include "SpriteSheetManager.h"
#include "HUD.h"
#include "LevelProperties.h"
#include "SMWFont.h"
#include "Keybindings.h"
#include "GameSession.h"
#include "StateManager.h"
#include "LevelSelectState.h"
#include "EndScreen.h"

#include "Platform.h"
#include "Pipe.h"
#include "Yoshi.h"

#include "Block.h"
#include "RotatingBlock.h"
#include "PrizeBlock.h"
#include "GrabBlock.h"
#include "Particle.h"
#include "ParticleManager.h"
#include "Player.h"
#include "SuperMushroom.h"
#include "KoopaShell.h"
#include "MidwayGate.h"
#include "GoalGate.h"
#include "Coin.h"
#include "PSwitch.h"
#include "Message.h"

#include "KoopaTroopa.h"
#include "MontyMole.h"
#include "CharginChuck.h"

// For every 1 real life second, 1.5 in game seconds elapse
const double Level::TIME_SCALE = 1.5;
const int Level::TIME_UP_WARNING = 100;
const int Level::MESSAGE_BLOCK_WARNING_TIME = 60;

Level::Level(Game* gamePtr, GameState* gameStatePtr, LevelProperties levelInfo, SessionInfo sessionInfo, Pipe* spawningPipePtr) :
	m_GamePtr(gamePtr), 
	INDEX(levelInfo.m_Index),
	WIDTH(levelInfo.m_Width),
	HEIGHT(levelInfo.m_Height),
	m_BmpBackgroundPtr((Bitmap*)levelInfo.m_BmpBackgroundPtr),
	IS_BACKGROUND_ANIMATED(levelInfo.m_NumberOfBackgroundAnimationFrames > -1),
	TOTAL_FRAMES_OF_BACKGROUND_ANIMATION(levelInfo.m_NumberOfBackgroundAnimationFrames),
	m_BmpForegroundPtr((Bitmap*)levelInfo.m_BmpForegroundPtr),
	m_BackgroundSong(levelInfo.m_BackgroundMusic),
	m_BackgroundSongFast(levelInfo.m_BackgroundMusicFast),
	TOTAL_TIME(levelInfo.m_TotalTime),
	m_GameStatePtr(gameStatePtr)
{
	m_ParticleManagerPtr = new ParticleManager();
	ResetMembers();

	if (sessionInfo.m_PlayerLives == -1) // The session info hasn't been set, use defaults
	{
		m_TimeRemaining = TOTAL_TIME;
		m_IsCheckpointCleared = false;
	}
	else // The session info has been set, use it
	{
		m_TimeRemaining = sessionInfo.m_TimeRemaining;
		m_SecondsElapsed = int((TOTAL_TIME - m_TimeRemaining + 1) * (1.0 / TIME_SCALE));
		m_IsCheckpointCleared = (sessionInfo.m_CheckpointCleared == 1);
		if (sessionInfo.m_PlayerRidingYoshi)
		{
			DOUBLE2 yoshiPos(0, 0);
			if (spawningPipePtr != nullptr) yoshiPos = spawningPipePtr->GetWarpToPosition();
			m_YoshiPtr = new Yoshi(yoshiPos, this, true);
		}
	}

	m_PlayerPtr = new Player(this, gameStatePtr, sessionInfo);

	m_ActLevelPtr = new PhysicsActor(DOUBLE2(0, 0), 0, BodyType::STATIC);
	m_ActLevelPtr->AddSVGFixture(levelInfo.m_LevelSVGFilePath, 0.0);
	m_ActLevelPtr->AddContactListener(this);
	m_ActLevelPtr->SetUserData(int(ActorId::LEVEL));

	m_CameraPtr = new Camera(Game::WIDTH, Game::HEIGHT, this);

	m_CoinsToBlocksTimer = SMWTimer(480);
	m_BackgroundAnimInfo.secondsPerFrame = 0.135;
}

Level::~Level()
{
	SoundManager::SetAllSongsPaused(true);
	LevelData::UnloadLevelData(INDEX);
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

	ResetMembers();
}

void Level::ResetMembers()
{
	m_Paused = false;
	m_TimeRemaining = TOTAL_TIME;
	m_IsCheckpointCleared = false;
	m_IsShowingEndScreen = false;
	m_TimeWarningPlayed = false;
	m_PSwitchTimeWarningPlayed = false;

	LevelData::UnloadLevelData(INDEX);
	ReadLevelData(INDEX);

	delete m_YoshiPtr;
	m_YoshiPtr = nullptr;
	m_ActiveMessagePtr = nullptr;

	m_SecondsElapsed = 0.0;
	m_ParticleManagerPtr->Reset();
	m_FinalExtraScore = {};

	SoundManager::RestartAndPauseSongs();
	SoundManager::PlaySong(m_BackgroundSong);
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
		SetPaused(false, false);
	}

	if (m_ActiveMessagePtr != nullptr)
	{
		if (m_ActiveMessagePtr->IsInputPaused())
		{
			// This message block won't be ticked unless we do it here
			m_ActiveMessagePtr->Tick(deltaTime);
			if (m_ActiveMessagePtr == nullptr) return; // If this is the last frame of outro anim, they are no longer our active message block
		}

		if (m_ActiveMessagePtr->IsShowingMessage())
		{
			if (m_PlayerPtr->IsDead() == false &&
				GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::X_BUTTON) ||
				GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::Y_BUTTON) ||
				GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::A_BUTTON) ||
				GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::B_BUTTON) ||
				GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::START_BUTTON))
			{
				m_ActiveMessagePtr->StartOutroAnimation();
				return;
			}
		}
	}
	else if (m_PlayerPtr->IsDead() == false &&
			m_GameStatePtr->ShowingSessionInfo() == false &&
			GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::START_BUTTON))
	{
		TogglePaused(true);
		SoundManager::PlaySoundEffect(SoundManager::Sound::GAME_PAUSE);
	}
	bool yoshiIsGrowing = m_YoshiPtr != nullptr && m_YoshiPtr->GetAnimationState() == Yoshi::AnimationState::BABY;
	if (m_Paused && 
		(yoshiIsGrowing ||
		 m_PlayerPtr->IsDead() ||
		 m_PlayerPtr->IsTransitioningPowerups() || 
		 m_PlayerPtr->GetAnimationState() == Player::AnimationState::IN_PIPE))
	{
		if (yoshiIsGrowing)
		{
			m_ParticleManagerPtr->Tick(deltaTime);
			m_YoshiPtr->Tick(deltaTime);
		}
		else
		{
			// NOTE: The player needs to still be ticked so they can animate
			m_CameraPtr->CalculateViewMatrix(this, deltaTime);
			m_PlayerPtr->Tick(deltaTime);
		}
		return;
	}
	else if (m_Paused || 
			(m_ActiveMessagePtr != nullptr && m_ActiveMessagePtr->IsInputPaused()))
	{
		return;
	}

	if (m_IsShowingEndScreen)
	{
		if (EndScreen::Tick())
		{
			StateManager* stateManagerPtr = m_GameStatePtr->GetStateManagerPtr();
			stateManagerPtr->SetState(new LevelSelectState(stateManagerPtr));
			m_IsShowingEndScreen = false;
		}
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
	for (size_t i = 0; i < m_EnemiesToBeRemovedPtrArr.size(); ++i)
	{
		if (m_EnemiesToBeRemovedPtrArr[i] != nullptr)
		{
			m_LevelDataPtr->RemoveEnemy(m_EnemiesToBeRemovedPtrArr[i]);
			m_EnemiesToBeRemovedPtrArr[i] = nullptr;
		}
	}

	if (m_PlayerPtr->GetExtraItemType() != Item::Type::NONE &&
		m_Paused && GAME_ENGINE->IsKeyboardKeyPressed(Keybindings::SELECT_BUTTON))
	{
		DOUBLE2 cameraOffset = GetCameraOffset(deltaTime);
		double xPos = cameraOffset.x + Game::WIDTH / 2.0;
		double yPos = cameraOffset.y + 25;

		m_PlayerPtr->ReleaseExtraItem(DOUBLE2(xPos, yPos));
	}
	
	if (IS_BACKGROUND_ANIMATED)
	{
		m_BackgroundAnimInfo.Tick(deltaTime);
		m_BackgroundAnimInfo.frameNumber %= (TOTAL_FRAMES_OF_BACKGROUND_ANIMATION * 2 - 2);
	}

	if (m_CoinsToBlocksTimer.Tick())
	{
		if (m_PSwitchTimeWarningPlayed == false && 
			m_CoinsToBlocksTimer.TotalFrames() - m_CoinsToBlocksTimer.FramesElapsed() <= MESSAGE_BLOCK_WARNING_TIME)
		{
			m_PSwitchTimeWarningPlayed = true;
			SoundManager::PlaySoundEffect(SoundManager::Sound::PSWITCH_TIME_WARNING);
		}

		if (m_CoinsToBlocksTimer.IsComplete())
		{
			TurnCoinsToBlocks(false);
		}
	}

	if (m_YoshiPtr != nullptr)
	{
		m_YoshiPtr->Tick(deltaTime);
	}

	m_PlayerPtr->Tick(deltaTime);
	m_ParticleManagerPtr->Tick(deltaTime);
	m_LevelDataPtr->TickItemsAndEnemies(deltaTime, this);

	m_CameraPtr->CalculateViewMatrix(this, deltaTime);
}

void Level::Paint()
{
	const MATRIX3X2 matCameraView = m_CameraPtr->GetViewMatrix();
	const MATRIX3X2 matTotalView = matCameraView *  Game::matIdentity;
	GAME_ENGINE->SetViewMatrix(matTotalView);

	const int bgWidth = m_BmpBackgroundPtr->GetWidth();
	const double cameraX = -matCameraView.orig.x;
	const double parallax = (1.0 - 0.50); // 50% of the speed of the camera
	int xo = int(cameraX * parallax);
	xo += (int(cameraX - xo) / bgWidth) * bgWidth;

	RECT2 bgSrcRect;
	if (IS_BACKGROUND_ANIMATED)
	{
		int frameToShow = m_BackgroundAnimInfo.frameNumber;
		if (frameToShow >= TOTAL_FRAMES_OF_BACKGROUND_ANIMATION)
		{
			frameToShow = TOTAL_FRAMES_OF_BACKGROUND_ANIMATION - m_BackgroundAnimInfo.frameNumber + 1;
		}

		const int bgWidth = m_BmpBackgroundPtr->GetWidth() / TOTAL_FRAMES_OF_BACKGROUND_ANIMATION;
		const int bgHeight = m_BmpBackgroundPtr->GetHeight();
		bgSrcRect = RECT2(frameToShow * bgWidth, 0, (frameToShow + 1) * bgWidth, bgHeight);
	}
	else
	{
		bgSrcRect = RECT2(0, 0, m_BmpBackgroundPtr->GetWidth(), m_BmpBackgroundPtr->GetHeight());
	}
	// Background
	GAME_ENGINE->DrawBitmap(m_BmpBackgroundPtr, DOUBLE2(xo, -32), bgSrcRect);
	if (Game::WIDTH - (cameraX - xo) < 0)
	{
		GAME_ENGINE->DrawBitmap(m_BmpBackgroundPtr, DOUBLE2(xo + bgWidth, -32), bgSrcRect);
	}
	m_LevelDataPtr->PaintEnemiesInBackground();
	if (m_PlayerPtr->GetAnimationState() == Player::AnimationState::IN_PIPE)
	{
		m_PlayerPtr->Paint();
	}

	// Foreground
	GAME_ENGINE->DrawBitmap(m_BmpForegroundPtr);
	m_LevelDataPtr->PaintItemsAndEnemies();
	if (m_YoshiPtr != nullptr && m_PlayerPtr->IsRidingYoshi() == false)
	{
		m_YoshiPtr->Paint();
	}
	if (m_PlayerPtr->GetAnimationState() != Player::AnimationState::IN_PIPE)
	{
		m_PlayerPtr->Paint();
	}
	m_LevelDataPtr->PaintItemsInForeground();
	m_ParticleManagerPtr->Paint();

	GAME_ENGINE->SetViewMatrix(Game::matIdentity);

	if (m_IsShowingEndScreen)
	{
		EndScreen::Paint();
	}

	if (Game::DEBUG_ZOOM_OUT)
	{
		GAME_ENGINE->SetColor(COLOR(250, 10, 10));
		GAME_ENGINE->DrawRect(0, 0, Game::WIDTH, Game::HEIGHT, 2.5);
	}

	PaintHUD();

	if (Game::DEBUG_SHOWING_CAMERA_INFO)
	{
		m_CameraPtr->DEBUGPaint();
	}

	if (Game::DEBUG_SHOWING_PLAYER_INFO)
	{
		GAME_ENGINE->SetColor(COLOR(0, 0, 0));
		GAME_ENGINE->SetFont(Game::Font9Ptr);
		int yo = 197;
		int dy = 9;
		GAME_ENGINE->DrawString(String("pos: ") + m_PlayerPtr->GetPosition().ToString(), 10, yo); yo += dy;
		GAME_ENGINE->DrawString(String("vel: ") + m_PlayerPtr->GetLinearVelocity().ToString(), 10, yo); yo += dy;
		GAME_ENGINE->DrawString(String("onGround: ") + String(m_PlayerPtr->IsOnGround() ? "true" : "false"), 10, yo); yo -= dy * 2;
		GAME_ENGINE->DrawString(String("t gb: ") + String(m_PlayerPtr->DEBUGIsTouchingGrabBlock() ? "true" : "false"), 125, yo); yo += dy;

		if (SoundManager::IsMuted())
		{
			GAME_ENGINE->DrawString(String("m"), 245, 198);
		}
	}

	GAME_ENGINE->SetViewMatrix(matTotalView);
}

void Level::PaintHUD()
{
	const int playerLives = m_PlayerPtr->GetLives();
	const int playerDragonCoins = m_PlayerPtr->GetDragonCoinsCollected();
	const int playerRedStars = m_PlayerPtr->GetRedStarsCollected();
	const int playerCoins = m_PlayerPtr->GetCoinsCollected();
	const int playerScore = m_PlayerPtr->GetScore();
	const Item::Type playerExtraItemType = m_PlayerPtr->GetExtraItemType();

	m_TimeRemaining = TOTAL_TIME - (int(m_SecondsElapsed * TIME_SCALE));
	if (m_TimeWarningPlayed == false && m_TimeRemaining <= TIME_UP_WARNING)
	{
		SpeedUpMusic();
	}

	int x = 15;
	int y = 15;
	RECT2 srcRect;
	Bitmap* hudBmpPtr = SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::HUD);

	// MARIO
	srcRect = RECT2(1, 1, 41, 9);
	GAME_ENGINE->DrawBitmap(hudBmpPtr, x, y, srcRect);
	
	// X
	x += 9;
	y += 8;
	srcRect = RECT2(10, 61, 10 + 7, 61 + 7);
	GAME_ENGINE->DrawBitmap(hudBmpPtr, x, y, srcRect);

	// LIVES
	x += 15;
	HUD::PaintSeveralDigitNumber(x, y, playerLives, false);

	// DRAGON COINS
	x += 24;
	y = 15;
	for (int i = 0; i < playerDragonCoins; ++i)
	{
		srcRect = RECT2(1, 60, 1 + 8, 60 + 8);
		GAME_ENGINE->DrawBitmap(hudBmpPtr, x, y, srcRect);
		x += 8;
	}

	// RED STAR
	y += 8;
	x = 70;
	srcRect = RECT2(19, 60, 19 + 8, 60 + 8);
	GAME_ENGINE->DrawBitmap(hudBmpPtr, x, y, srcRect);

	// X
	x += 10;
	y += 2;
	srcRect = RECT2(10, 61, 10 + 7, 61 + 7);
	GAME_ENGINE->DrawBitmap(hudBmpPtr, x, y, srcRect);

	// NUMBER OF STARS
	x += 24;
	y -= 7;
	HUD::PaintSeveralDigitLargeNumber(x, y, playerRedStars);

	// ITEM BOX
	x += 10;
	y -= 7;
	srcRect = RECT2(36, 52, 36 + 28, 52 + 28);
	GAME_ENGINE->DrawBitmap(hudBmpPtr, x, y, srcRect);

	// TIME LABEL
	x += 37;
	y = 15;
	srcRect = RECT2(1, 52, 1 + 24, 52 + 7);
	GAME_ENGINE->DrawBitmap(hudBmpPtr, x, y, srcRect);

	// TIME VALUE
	y += 9;
	x += 16;
	HUD::PaintSeveralDigitNumber(x, y, m_TimeRemaining, true);

	// COIN LABEL
	x += 33;
	y = 15;
	srcRect = RECT2(1, 60, 1 + 16, 60 + 8);
	GAME_ENGINE->DrawBitmap(hudBmpPtr, x, y, srcRect);

	// COINS
	x += 30;
	HUD::PaintSeveralDigitNumber(x, y, playerCoins, false);

	// SCORE
	y += 8;
	HUD::PaintSeveralDigitNumber(x, y, playerScore, false);
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

void Level::CollidePlayerWithBlock(DOUBLE2 blockCenterPos, DOUBLE2 playerFeet, bool& enableContactRef)
{
	bool playerIsBesideBlock = (playerFeet.x + m_PlayerPtr->GetWidth() / 2.0 < blockCenterPos.x - Block::WIDTH / 2.0) ||
		(playerFeet.x - m_PlayerPtr->GetWidth() / 2.0 > blockCenterPos.x + Block::WIDTH / 2.0);

	if (playerIsBesideBlock)
	{
		enableContactRef = false;

		bool playersFeetAreBelowTopOfBlock = (playerFeet.y >= blockCenterPos.y - Block::HEIGHT / 2);
		if (playersFeetAreBelowTopOfBlock)
		{
			m_PlayerPtr->SetLinearVelocity(DOUBLE2(0, m_PlayerPtr->GetLinearVelocity().y));
		}
	}
}

void Level::PreSolve(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr, bool& enableContactRef)
{
	const DOUBLE2 playerFeet(m_PlayerPtr->GetPosition().x, m_PlayerPtr->GetPosition().y + m_PlayerPtr->GetHeight() / 2);

	switch (actOtherPtr->GetUserData())
	{
	case int(ActorId::PLAYER):
	{
		if (m_PlayerPtr->IsDead())
		{
			break;
		}

		const bool playerIsRising = m_PlayerPtr->GetLinearVelocity().y < -0.001;

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
		{
			if (playerIsRising)
			{
				m_PlayerPtr->SetLinearVelocity(DOUBLE2(0, m_PlayerPtr->GetLinearVelocity().y));
				enableContactRef = false;
			}
		} break;
		case int(ActorId::PIPE):
		{
			if (playerIsRising && 
				((Pipe*)actThisPtr->GetUserPointer())->GetOrientation() != Pipe::Orientation::DOWN)
			{
				m_PlayerPtr->SetLinearVelocity(DOUBLE2(0, m_PlayerPtr->GetLinearVelocity().y));
				enableContactRef = false;
			}
		} break;
		case int(ActorId::ITEM):
		{
			Item* itemPtr = (Item*)actThisPtr->GetUserPointer();
			switch (itemPtr->GetType())
			{
			case Item::Type::PRIZE_BLOCK:
			case Item::Type::MESSAGE_BLOCK:
			case Item::Type::EXCLAMATION_MARK_BLOCK:
			case Item::Type::CLOUD_BLOCK:
			{
				CollidePlayerWithBlock(itemPtr->GetPosition(), playerFeet, enableContactRef);
			} break;
			case Item::Type::ROTATING_BLOCK:
			{
				if (((RotatingBlock*)itemPtr)->IsRotating())
				{
					enableContactRef = false;
					return;
				}
				else
				{
					CollidePlayerWithBlock(itemPtr->GetPosition(), playerFeet, enableContactRef);
				}
			} break;
			case Item::Type::KOOPA_SHELL:
			{
				enableContactRef = false;
			} break;
			case Item::Type::GRAB_BLOCK:
			{
				const bool grabBlockIsBeingHeld = m_PlayerPtr->GetHeldItemPtr() == itemPtr;
				if (grabBlockIsBeingHeld) enableContactRef = false;
			} break;
			case Item::Type::COIN:
			{
				if (((Coin*)itemPtr)->IsBlock())
				{
					CollidePlayerWithBlock(itemPtr->GetPosition(), playerFeet, enableContactRef);
				}
			} break;
			case Item::Type::P_SWITCH:
			{
				if (playerFeet.y > actThisPtr->GetPosition().y) enableContactRef = false;
			} break;
			}
		} break;
		case int(ActorId::YOSHI):
		{
			if (m_PlayerPtr->IsRidingYoshi() ||
				m_YoshiPtr->IsHatching() || 
				m_PlayerPtr->IsAirborne() == false || // On the ground
				(m_PlayerPtr->IsAirborne() && m_PlayerPtr->GetLinearVelocity().y <= 0)) // Going upwards
			{
				enableContactRef = false;
			}
		} break;
		case int(ActorId::ENEMY):
		{
			if (m_PlayerPtr->IsInvincible()) enableContactRef = false;
		} break;
		}
	} break;
	case int(ActorId::ENEMY):
	{
		Enemy* enemyPtr = (Enemy*)actOtherPtr->GetUserPointer();

		// Fixes chargin chuck's jumping:
		if (enemyPtr->GetType() == Enemy::Type::CHARGIN_CHUCK)
		{
			CharginChuck* charginChuckPtr = (CharginChuck*)enemyPtr;
			if (charginChuckPtr->GetAnimationState() == CharginChuck::AnimationState::JUMPING)
			{
				enableContactRef = false;
			}
		}

		switch (actThisPtr->GetUserData())
		{
		case int(ActorId::PLATFORM):
		{
			double enemyWidth = enemyPtr->GetWidth();
			double enemyHeight = enemyPtr->GetHeight();
			if (ActorCanPassThroughPlatform(actThisPtr, enemyPtr->GetPosition(), enemyWidth, enemyHeight))
			{
				enableContactRef = false;
			}
		} break;
		case int(ActorId::ENEMY):
		{
			Enemy* thisEnemyPtr = (Enemy*)actThisPtr->GetUserPointer();
			if (enemyPtr->GetType() == Enemy::Type::MONTY_MOLE &&
				thisEnemyPtr->GetType() == Enemy::Type::MONTY_MOLE)
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
		case Item::Type::P_SWITCH:
		{
			if (actThisPtr->GetUserData() == int(ActorId::ITEM))
			{
				Item* thisItemPtr = (Item*)actThisPtr->GetUserPointer();
				if (thisItemPtr->GetType() == Item::Type::ROTATING_BLOCK)
				{
					// NOTE: Only let p-switches fall through if the rotating block is spinning
					if (((RotatingBlock*)thisItemPtr)->IsRotating())
					{
						enableContactRef = false;
					}
				}
			}
		} break;
		case Item::Type::KOOPA_SHELL:
		{
			KoopaShell* koopaShellPtr = (KoopaShell*)otherItemPtr;
			// When the player is holding a shell, it doesn't collide with anything except enemies
			const bool shellIsBeingHeld = m_PlayerPtr->GetHeldItemPtr() == koopaShellPtr;
			const bool shellIsMovingUpwards = actOtherPtr->GetLinearVelocity().y < 0.0;
			const bool shellIsHittingYoshi = actThisPtr->GetUserData() == int(ActorId::YOSHI);
			const bool shellIsHittingYoshiWithPlayer = actThisPtr->GetUserData() == int(ActorId::PLAYER) &&
														m_PlayerPtr->IsRidingYoshi();

			if ((actThisPtr->GetUserData() != int(ActorId::ENEMY) && shellIsBeingHeld) ||
				shellIsMovingUpwards || 
				shellIsHittingYoshi || 
				shellIsHittingYoshiWithPlayer)
			{
				enableContactRef = false;
			}
		} break;
		}
	} break;
	case int(ActorId::PLATFORM):
	{
		if (actThisPtr->GetUserData() == int(ActorId::ITEM))
		{
			Item* itemPtr = (Item*)actThisPtr->GetUserPointer();
			if (itemPtr->GetType() == Item::Type::KOOPA_SHELL)
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

		const bool playerIsRising = actOtherPtr->GetLinearVelocity().y < -0.001;
		const DOUBLE2 playerFeet = DOUBLE2(actOtherPtr->GetPosition().x, actOtherPtr->GetPosition().y + ((Player*)actOtherPtr)->GetHeight() / 2);

		switch (actThisPtr->GetUserData())
		{
		case int(ActorId::ITEM):
		{
			Item* itemPtr = (Item*)actThisPtr->GetUserPointer();
			switch (itemPtr->GetType())
			{
			case Item::Type::COIN:
			{
				if (((Coin*)itemPtr)->IsBlock())
				{
					break;
				}
			} // NOTE: No break here! Let it fallthrough
			case Item::Type::DRAGON_COIN:
			case Item::Type::SUPER_MUSHROOM:
			case Item::Type::STAR:
			case Item::Type::FIRE_FLOWER:
			case Item::Type::CAPE_FEATHER:
			case Item::Type::POWER_BALLOON:
			case Item::Type::ONE_UP_MUSHROOM:
			case Item::Type::THREE_UP_MOON:
			{
				m_PlayerPtr->OnItemPickup(itemPtr);
				m_ItemsToBeRemovedPtrArr.push_back(itemPtr);
			} break;
			case Item::Type::PRIZE_BLOCK:
			case Item::Type::EXCLAMATION_MARK_BLOCK:
			case Item::Type::MESSAGE_BLOCK:
			case Item::Type::CLOUD_BLOCK:
			{
				const bool playerCenterIsBelowBlock = m_PlayerPtr->GetPosition().y > (actThisPtr->GetPosition().y + Block::HEIGHT / 2);
				const bool playerCenterIsAboveBlock = m_PlayerPtr->GetPosition().y < (actThisPtr->GetPosition().y - Block::HEIGHT / 2);

				if (playerIsRising && playerCenterIsBelowBlock)
				{
					((Block*)itemPtr)->Hit();

					// NOTE: This line prevents the player from slowly floating down after hitting a block
					m_PlayerPtr->SetLinearVelocity(DOUBLE2(m_PlayerPtr->GetLinearVelocity().x, 0.0));
				}
			} break;
			case Item::Type::ROTATING_BLOCK:
			{
				RotatingBlock* rotatingBlockPtr = (RotatingBlock*)itemPtr;
				if (rotatingBlockPtr->IsRotating()) break;

				const bool playerCenterIsBelowBlock = m_PlayerPtr->GetPosition().y > (rotatingBlockPtr->GetPosition().y + Block::HEIGHT / 2);
				const bool playerCenterIsAboveBlock = m_PlayerPtr->GetPosition().y < (rotatingBlockPtr->GetPosition().y - Block::HEIGHT / 2);

				if (playerIsRising && playerCenterIsBelowBlock)
				{
					rotatingBlockPtr->Hit();

					// NOTE: This line prevents the player from slowly floating down after hitting a block
					m_PlayerPtr->SetLinearVelocity(DOUBLE2(m_PlayerPtr->GetLinearVelocity().x, 0.0));
				}
			} break;
			case Item::Type::GRAB_BLOCK:
			{
				GrabBlock* grabBlockPtr = (GrabBlock*)itemPtr;
				if (grabBlockPtr->IsFlashing())
				{
					const DOUBLE2 grabBlockVel(300 * m_PlayerPtr->GetDirectionFacing(), 0);
					grabBlockPtr->SetMoving(grabBlockVel);
				}
				else
				{
					m_PlayerPtr->SetTouchingGrabBlock(true, grabBlockPtr);
				}
			} break;
			case Item::Type::KOOPA_SHELL:
			{
				KoopaShell* koopaShellPtr = (KoopaShell*)itemPtr;
				if (m_PlayerPtr->GetAnimationState() == Player::AnimationState::SPIN_JUMPING)
				{
					koopaShellPtr->Stomp();
				}
				else if (koopaShellPtr->IsMoving())
				{
					if (m_PlayerPtr->GetAnimationState() == Player::AnimationState::FALLING)
					{
						koopaShellPtr->SetMoving(false);
						m_PlayerPtr->Bounce(false);
					}
					else
					{
						m_PlayerPtr->TakeDamage();
					}
				}
				else if (m_PlayerPtr->IsRunButtonHeldDown() && m_PlayerPtr->IsHoldingItem() == false)
				{
					if (koopaShellPtr->IsFallingOffScreen() == false)
					{
						koopaShellPtr->SetMoving(false);
						m_PlayerPtr->AddItemToBeHeld(itemPtr);
					}
				}
				else
				{
					bool wasThrown = m_PlayerPtr->GetHeldItemPtr() != nullptr;
					m_PlayerPtr->KickShell(koopaShellPtr, wasThrown);
				}
			} break;
			case Item::Type::BEANSTALK:
			{
				m_PlayerPtr->SetOverlappingWithBeanstalk(true);
			} break;
			case Item::Type::MIDWAY_GATE:
			{
				MidwayGate* midwayGatePtr = (MidwayGate*)itemPtr;
				if (midwayGatePtr->IsHit() == false)
				{
					m_PlayerPtr->MidwayGatePasshrough();
					midwayGatePtr->Hit();
					m_IsCheckpointCleared = true;
				}
			} break;
			case Item::Type::GOAL_GATE:
			{
				GoalGate* goalGatePtr = (GoalGate*)itemPtr;
				if (goalGatePtr->IsHit() == false)
				{
					goalGatePtr->Hit();
				}
			} break;
			case Item::Type::P_SWITCH:
			{
				PSwitch* pSwitchPtr = (PSwitch*)itemPtr;

				if (m_PlayerPtr->IsRunButtonHeldDown() && m_PlayerPtr->IsHoldingItem() == false)
				{
					m_PlayerPtr->AddItemToBeHeld(pSwitchPtr);
				}
				else
				{
					if (m_PlayerPtr->GetLinearVelocity().y > 0)
					{
						pSwitchPtr->Hit();
						TurnCoinsToBlocks(true);
					}
				}
			} break;
			}
		} break;
		case int(ActorId::PIPE):
		{
			m_PlayerPtr->SetTouchingPipe(true, (Pipe*)actThisPtr->GetUserPointer());
		} break;
		case int(ActorId::ENEMY):
		{
			Enemy* enemyPtr = (Enemy*)actThisPtr->GetUserPointer();
			const DOUBLE2 enemyFeet = enemyPtr->GetPosition() + DOUBLE2(0, enemyPtr->GetHeight() / 2);
			switch (enemyPtr->GetType())
			{
			case Enemy::Type::KOOPA_TROOPA:
			{
				KoopaTroopa* koopaTroopaPtr = (KoopaTroopa*)enemyPtr;
				if (playerFeet.y < koopaTroopaPtr->GetPosition().y)
				{
					if (m_PlayerPtr->IsHoldingItem() == false &&
						m_PlayerPtr->GetAnimationState() == Player::AnimationState::SPIN_JUMPING)
					{
						koopaTroopaPtr->StompKill();
						m_PlayerPtr->SetLinearVelocity(DOUBLE2(m_PlayerPtr->GetLinearVelocity().x, -58));
					}
					else
					{
						koopaTroopaPtr->HeadBonk();
						m_PlayerPtr->Bounce(false);
					}
				}
				else
				{
					// NOTE: If the koopa is shelless (aka laying helplessly and crying on the ground) then
					// all the player needs to do is touch them and they die
					// Once the shelless koopa starts walking (ANIMATION_STATE::WALKING_SHELLESS) then the player
					// needs to jump on their head, the player will die if they touch the side of a walking shelless koopa
					if (koopaTroopaPtr->GetAnimationState() == KoopaTroopa::AnimationState::SHELLESS)
					{
						koopaTroopaPtr->ShellHit();
					}
					else
					{
						m_PlayerPtr->TakeDamage();
					}
				}
			} break;
			case Enemy::Type::MONTY_MOLE:
			{
				MontyMole* montyMolePtr = (MontyMole*)enemyPtr;
				if (montyMolePtr->IsAlive())
				{
					if (playerFeet.y < montyMolePtr->GetPosition().y)
					{
						if (m_PlayerPtr->GetAnimationState() == Player::AnimationState::SPIN_JUMPING)
						{
							montyMolePtr->StompKill();
							m_PlayerPtr->SetLinearVelocity(DOUBLE2(m_PlayerPtr->GetLinearVelocity().x, 0));
						}
						else
						{
							montyMolePtr->HeadBonk();
							m_PlayerPtr->Bounce(false);
						}
					}
					else
					{
						m_PlayerPtr->TakeDamage();
					}
				}
			} break;
			case Enemy::Type::PIRHANA_PLANT:
			{
				m_PlayerPtr->TakeDamage();
			} break;
			case Enemy::Type::CHARGIN_CHUCK:
			{
				CharginChuck* charginChuckPtr = (CharginChuck*)enemyPtr;
				if (charginChuckPtr->GetAnimationState() != CharginChuck::AnimationState::DEAD &&
					m_PlayerPtr->IsAirborne())
				{
					charginChuckPtr->HeadBonk();
					const int horizontalDir = playerFeet.x < charginChuckPtr->GetPosition().x ? -1 : 1;
					m_PlayerPtr->Bounce(true, horizontalDir);
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
			if (m_YoshiPtr->IsHatching() == false && 
				m_PlayerPtr->IsRidingYoshi() == false && 
				m_PlayerPtr->IsHoldingItem() == false &&
				(m_PlayerPtr->IsAirborne() && m_PlayerPtr->GetLinearVelocity().y >= 0))
			{
				m_PlayerPtr->RideYoshi(m_YoshiPtr);
			}
		} break;
		}
	} break;
	case int(ActorId::ITEM):
	{
		Item* otherItemPtr = (Item*)actOtherPtr->GetUserPointer();
		switch (otherItemPtr->GetType())
		{
		case Item::Type::KOOPA_SHELL:
		{
			KoopaShell* otherKoopaShellPtr = (KoopaShell*)otherItemPtr;
			switch (actThisPtr->GetUserData())
			{
			case int(ActorId::ITEM):
			{
				Item* thisItemPtr = (Item*)actThisPtr->GetUserPointer();
				if (thisItemPtr->IsBlock() && m_PlayerPtr->GetHeldItemPtr() != otherKoopaShellPtr)
				{
					Block* blockPtr = (Block*)thisItemPtr;
					DOUBLE2 koopaShellPos = otherKoopaShellPtr->GetPosition();
					DOUBLE2 blockPos = blockPtr->GetPosition();
					bool shellBelowBlock = koopaShellPos.y > blockPos.y;
					bool shellBesideBlock = koopaShellPos.x + KoopaShell::WIDTH / 2 <= blockPos.x - Block::WIDTH / 2 ||
						koopaShellPos.x - KoopaShell::WIDTH / 2 >= blockPos.x + Block::WIDTH / 2;

					if (shellBelowBlock || shellBesideBlock)
					{
						DOUBLE2 koopaShellVel = actOtherPtr->GetLinearVelocity();
						otherKoopaShellPtr->SetLinearVelocity(DOUBLE2(-koopaShellVel.x, koopaShellVel.y));
						// When a shell hits a block it "hits" it as if the player hit it with their head
						blockPtr->Hit();
					}
				}
				else if (thisItemPtr->GetType() == Item::Type::KOOPA_SHELL)
				{
					KoopaShell* thisKoopaShellPtr = (KoopaShell*)thisItemPtr;
					if (m_PlayerPtr->GetHeldItemPtr() == otherKoopaShellPtr)
					{
						m_PlayerPtr->DropHeldItem();
						thisKoopaShellPtr->ShellHit(m_PlayerPtr->GetDirectionFacing());
						m_PlayerPtr->AddScore(1000, false, thisKoopaShellPtr->GetPosition());
					}
					else if (!otherKoopaShellPtr->IsMoving())
					{
						otherKoopaShellPtr->ShellHit();
					}
				}
			} break;
			case int(ActorId::ENEMY):
			{
				Enemy* enemyPtr = (Enemy*)actThisPtr->GetUserPointer();
				switch (enemyPtr->GetType())
				{
				case Enemy::Type::KOOPA_TROOPA:
				{
					if (m_PlayerPtr->GetHeldItemPtr() == otherKoopaShellPtr)
					{
						m_PlayerPtr->DropHeldItem();
						((KoopaTroopa*)enemyPtr)->ShellHit(true);
					}
					else if (otherKoopaShellPtr->IsMoving() || otherKoopaShellPtr->IsBouncing())
					{
						((KoopaTroopa*)enemyPtr)->ShellHit();
					}
				} break;
				}
			} break;
			case int(ActorId::PLAYER):
			{
				if (m_PlayerPtr->IsRidingYoshi())
				{
					m_PlayerPtr->DismountYoshi(true);
				}
			} break;
			}
		} break;
		case Item::Type::GRAB_BLOCK:
		{
			GrabBlock* grabBlockPtr = (GrabBlock*)otherItemPtr;
			if (actThisPtr->GetUserData() == int(ActorId::ITEM)) 
			{
				Item* thisItemPtr = (Item*)actThisPtr->GetUserPointer();
				if (thisItemPtr->GetType() == Item::Type::PRIZE_BLOCK)
				{
					PrizeBlock* prizeBlockPtr = (PrizeBlock*)thisItemPtr;
					if (prizeBlockPtr->IsFlying())
					{
						prizeBlockPtr->Hit();
					}
				}
			}
			else 
			{
				const int grabBlockDir = actOtherPtr->GetLinearVelocity().x > 0 ? Direction::RIGHT : Direction::LEFT;
				const DOUBLE2 point1 = grabBlockPtr->GetPosition();
				const DOUBLE2 point2 = grabBlockPtr->GetPosition() + DOUBLE2((Block::WIDTH / 2 + 3) * grabBlockDir, 0);
				DOUBLE2 intersection, normal;
				double fraction = -1.0;
				int collisionBits = Level::LEVEL | Level::BLOCK;

				if (Raycast(point1, point2, collisionBits, intersection, normal, fraction))
				{
					// Explode when hitting things head on
					grabBlockPtr->Explode();
				}
			}
		} break;
		case Item::Type::FIREBALL:
		{
			if (actThisPtr->GetUserData() == int(ActorId::ENEMY))
			{
				Enemy* enemyPtr = (Enemy*)actThisPtr->GetUserPointer();
				m_EnemiesToBeRemovedPtrArr.push_back(enemyPtr);
				SoundManager::PlaySoundEffect(SoundManager::Sound::SHELL_KICK);
				m_PlayerPtr->AddScore(200, false, enemyPtr->GetPosition() + DOUBLE2(0, -10));
			}
		} break;
		}
	} break;
	case int(ActorId::YOSHI_TOUNGE):
	{
		switch (actThisPtr->GetUserData())
		{
		case int(ActorId::ENEMY):
		{
			if (m_PlayerPtr->IsRidingYoshi() &&
				m_YoshiPtr->IsTongueStuckOut() == false) 
			{
				m_PlayerPtr->TakeDamage();
			}
			else
			{
				Enemy* enemyPtr = (Enemy*)actThisPtr->GetUserPointer();
				m_YoshiPtr->TongueTouchedEnemy(enemyPtr);
			}
		} break;
		case int(ActorId::ITEM):
		{
			Item* itemPtr = (Item*)actThisPtr->GetUserPointer();
			m_YoshiPtr->TongueTouchedItem(itemPtr);
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
			case Item::Type::BEANSTALK:
			{
				m_PlayerPtr->SetOverlappingWithBeanstalk(false);
			} break;
			case Item::Type::GRAB_BLOCK:
			{
				m_PlayerPtr->SetTouchingGrabBlock(false, (GrabBlock*)otherItemPtr);
			} break;
			}
		} break;
		case int(ActorId::PIPE):
		{
			m_PlayerPtr->SetTouchingPipe(false);
		} break;
		}
	}
}

bool Level::Raycast(DOUBLE2 point1, DOUBLE2 point2, int collisionBits, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef)
{
	if (collisionBits & LEVEL)
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

	if (collisionBits & ENEMY)
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

	if (collisionBits & PLAYER)
	{
		if (m_PlayerPtr->Raycast(point1, point2, intersectionRef, normalRef, fractionRef))
		{
			return true;
		}
	}

	if (collisionBits & BLOCK || collisionBits & SHELL)
	{
		std::vector<Item*> itemsPtrArr = m_LevelDataPtr->GetItems();
		for (size_t i = 0; i < itemsPtrArr.size(); ++i)
		{
			if (itemsPtrArr[i] != nullptr)
			{
				bool blockCollision = collisionBits & BLOCK && itemsPtrArr[i]->IsBlock();

				if (blockCollision && itemsPtrArr[i]->GetType() == Item::Type::ROTATING_BLOCK)
				{
					if (((RotatingBlock*)itemsPtrArr[i])->IsRotating())
					{
						blockCollision = false;
					}
				}

				bool shellCollision = collisionBits & SHELL && itemsPtrArr[i]->GetType() == Item::Type::KOOPA_SHELL;

				if (blockCollision || shellCollision)
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

void Level::TriggerEndScreen(int barHitHeight)
{
	m_IsShowingEndScreen = true;

	m_FinalExtraScore.m_FinalTimeRemaining = m_TimeRemaining;
	m_FinalExtraScore.m_ScoreShowing = 50 * m_TimeRemaining;
	m_FinalExtraScore.m_BonusScoreShowing = barHitHeight;
	
	EndScreen::Initalize(m_PlayerPtr, m_CameraPtr, m_FinalExtraScore.m_FinalTimeRemaining,
		m_FinalExtraScore.m_ScoreShowing, m_FinalExtraScore.m_BonusScoreShowing);

	SoundManager::PlaySoundEffect(SoundManager::Sound::COURSE_CLEAR_FANFARE);
	SoundManager::SetAllSongsPaused(true);
}

void Level::AddParticle(Particle* particlePtr)
{
	m_ParticleManagerPtr->AddParticle(particlePtr);
}

void Level::RemoveParticle(Particle* particlePtr)
{
	m_ParticleManagerPtr->RemoveParticle(particlePtr);
}

void Level::WarpPlayerToPipe(int pipeIndex)
{
	Pipe* pipePtr = m_LevelDataPtr->GetPipeWithIndex(pipeIndex);
	m_PlayerPtr->SetPosition(pipePtr->GetWarpToPosition());
	m_PlayerPtr->SetExitingPipe(pipePtr);
	SoundManager::PlaySoundEffect(SoundManager::Sound::PLAYER_DAMAGE);
}

DOUBLE2 Level::GetCameraOffset(double deltaTime) 
{
	return m_CameraPtr->GetOffset(this, deltaTime);
}

Player* Level::GetPlayer() const
{
	return m_PlayerPtr;
}

void Level::GiveItemToPlayer(Item* itemPtr)
{
	m_PlayerPtr->OnItemPickup(itemPtr);
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

void Level::AddItem(Item* newItemPtr, bool addContactListener)
{
	if (addContactListener) newItemPtr->AddContactListener(this);
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

void Level::TogglePaused(bool pauseSongs)
{
	SetPaused(!m_Paused, pauseSongs);
}

void Level::SetPaused(bool paused, bool pauseSongs)
{
	m_Paused = paused;

	m_PlayerPtr->SetPaused(m_Paused);
	m_LevelDataPtr->SetItemsAndEnemiesPaused(m_Paused);

	if (m_YoshiPtr != nullptr)
	{
		m_YoshiPtr->SetPaused(paused);
	}

	if (pauseSongs)
	{
		SoundManager::SetSongPaused(m_BackgroundSong, paused);
	}
}

void Level::SpeedUpMusic()
{
	m_TimeWarningPlayed = true;
	SoundManager::SetSongPaused(m_BackgroundSong, true);
	SoundManager::PlaySong(m_BackgroundSongFast);

	SoundManager::PlaySoundEffect(SoundManager::Sound::TIME_WARNING);
}

void Level::SetActiveMessage(Message* activeMessagePtr)
{
	m_ActiveMessagePtr = activeMessagePtr;
}

void Level::TurnCoinsToBlocks(bool toBlocks)
{
	std::vector<Item*> itemsPtrArr = m_LevelDataPtr->GetItems();
	for (size_t i = 0; i < itemsPtrArr.size(); ++i)
	{
		if (itemsPtrArr[i] != nullptr && itemsPtrArr[i]->GetType() == Item::Type::COIN)
		{
			((Coin*)itemsPtrArr[i])->TurnToBlock(toBlocks);
		}
	}
	
	if (toBlocks) 
	{
		m_CoinsToBlocksTimer.Start();
		// LATER: play sound here
		// LATER: play song here
	}
	else
	{
		// LATER: play sound here
	}
}

bool Level::IsPaused() const
{
	return m_Paused;
}

Yoshi* Level::GetYoshiPtr()
{
	return m_YoshiPtr;
}

double Level::GetWidth() const
{
	return WIDTH;
}

double Level::GetHeight() const
{
	return HEIGHT;
}

bool Level::IsCheckpointCleared() const
{
	return m_IsCheckpointCleared;
}

int Level::GetTimeRemaining() const
{
	return m_TimeRemaining;
}

void Level::SetPausedTimer(int duration, bool pauseSongs)
{
	m_GamePausedTimer = SMWTimer(duration);
	m_GamePausedTimer.Start();
	if (m_Paused == false)
	{
		SetPaused(true, pauseSongs);
	}
}
