#include "stdafx.h"

#include "Level.h"
#include "Game.h"
#include "GameState.h"
#include "Camera.h"
#include "LevelData.h"
#include "SpriteSheetManager.h"
#include "HUD.h"
#include "LevelInfo.h"
#include "SMWFont.h"
#include "Keybindings.h"
#include "GameSession.h"

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

Level::Level(Game* gamePtr, GameState* gameStatePtr, LevelInfo levelInfo, SessionInfo sessionInfo) :
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
	m_PlayerPtr = new Player(this, gameStatePtr);

	m_ActLevelPtr = new PhysicsActor(DOUBLE2(0, 0), 0, BodyType::STATIC);
	m_ActLevelPtr->AddSVGFixture(levelInfo.m_LevelSVGFilePath, 0.0);
	m_ActLevelPtr->AddContactListener(this);
	m_ActLevelPtr->SetUserData(int(ActorId::LEVEL));

	m_CameraPtr = new Camera(Game::WIDTH, Game::HEIGHT, this);
	m_ParticleManagerPtr = new ParticleManager();

	m_CoinsToBlocksTimer = SMWTimer(480);
	m_BackgroundAnimInfo.secondsPerFrame = 0.135;

	ResetMembers();
}

Level::~Level()
{
	SoundManager::SetAllSongsPaused(true);
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

	m_SecondsElapsed = 0.0;
	m_TimeRemaining = TOTAL_TIME;

	m_IsCheckpointCleared = false;

	m_TimeWarningPlayed = false;
	m_PSwitchTimeWarningPlayed = false;

	LevelData::UnloadLevel(INDEX);
	ReadLevelData(INDEX);

	delete m_YoshiPtr;
	m_YoshiPtr = nullptr;
	m_ActiveMessagePtr = nullptr;

	m_IsShowingEndScreen = false;
	m_FramesShowingEndScreen = 0;

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
	MATRIX3X2 matCameraView = m_CameraPtr->GetViewMatrix();
	MATRIX3X2 matTotalView = matCameraView *  Game::matIdentity;
	GAME_ENGINE->SetViewMatrix(matTotalView);

	int bgWidth = m_BmpBackgroundPtr->GetWidth();
	double cameraX = -matCameraView.orig.x;
	double parallax = (1.0 - 0.50); // 50% of the speed of the camera
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

		int bgWidth = m_BmpBackgroundPtr->GetWidth() / TOTAL_FRAMES_OF_BACKGROUND_ANIMATION;
		int bgHeight = m_BmpBackgroundPtr->GetHeight();
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
		PaintEndScreen();
	}

#if SMW_DEBUG_ZOOM_OUT
	GAME_ENGINE->SetColor(COLOR(250, 10, 10));
	GAME_ENGINE->DrawRect(0, 0, Game::WIDTH, Game::HEIGHT, 2.5);
#endif

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

void Level::PaintEndScreen()
{
	const int fadeTransition = 1600; // after this many frames, the transition will occur
	const int drumrollStart = fadeTransition + 1600;
	const int fadeFromBlackTransition = fadeTransition + 3550;
	const int backToClearTransition = fadeTransition + fadeFromBlackTransition;
	const int resetGameTransition = backToClearTransition + 600;

	++m_FramesShowingEndScreen;

	if (m_FramesShowingEndScreen < fadeTransition)
	{
		// Fade from clear to black
		int alpha = int((double(m_FramesShowingEndScreen) / double(fadeTransition)) * 255);
		GAME_ENGINE->SetColor(COLOR(0, 0, 0, alpha));
		GAME_ENGINE->FillRect(0, 0, Game::WIDTH, Game::HEIGHT);
	}
	else 
	{
		if (m_FramesShowingEndScreen == drumrollStart)
		{
			SoundManager::PlaySoundEffect(SoundManager::Sound::DRUMROLL);
		}

		GAME_ENGINE->SetColor(COLOR(0, 0, 0));
		GAME_ENGINE->FillRect(0, 0, Game::WIDTH, Game::HEIGHT);

		// Once the drumroll starts, the final score begins "moving" to the score in the top right
		if (m_FramesShowingEndScreen > drumrollStart)
		{
			if (m_FinalExtraScore.m_ScoreShowing > 0)
			{
				int delta = 50;
				if (m_FinalExtraScore.m_ScoreShowing - delta < 0) delta = m_FinalExtraScore.m_ScoreShowing;

				m_FinalExtraScore.m_ScoreShowing -= delta;
				m_PlayerPtr->AddScore(delta, false);
			}

			if (m_FinalExtraScore.m_BonusScoreShowing > 0)
			{
				m_FinalExtraScore.m_BonusScoreShowing -= 1;
				m_PlayerPtr->AddRedStars(1);
			}
		}

		int left = 75;
		int top = 60;

		Bitmap* hudBmpPtr = SpriteSheetManager::GetBitmapPtr(SpriteSheetManager::HUD);

		// MARIO
		RECT2 srcRect = RECT2(1, 1, 41, 9);
		GAME_ENGINE->DrawBitmap(hudBmpPtr, left + 32, top, srcRect);
		top += 16;

		// COURSE CLEAR
		GAME_ENGINE->SetColor(COLOR(255, 255, 255));
		SMWFont::PaintPhrase("COURSE CLEAR!", left, top, SMWFont::OUTLINED);
		top += 22;

		// Bonus time score
		SMWFont::PaintPhrase("@" + std::to_string(m_FinalExtraScore.m_FinalTimeRemaining) + "*50=", left, top, SMWFont::OUTLINED);
		std::stringstream scoreShowingStream;
		scoreShowingStream << std::setw(5) << std::setfill(' ') << std::to_string(m_FinalExtraScore.m_ScoreShowing);
		SMWFont::PaintPhrase(scoreShowingStream.str(), left + 65, top, SMWFont::OUTLINED);

		// Bonus red stars
		if (m_FinalExtraScore.m_BonusScoreShowing > 0)
		{
			left += 4;
			top += 28;
			SMWFont::PaintPhrase("BONUS!", left, top, SMWFont::OUTLINED);

			// RED STAR
			left += 55;
			srcRect = RECT2(19, 60, 19 + 8, 60 + 8);
			GAME_ENGINE->DrawBitmap(hudBmpPtr, left, top, srcRect);

			// X
			left += 10;
			top += 2;
			srcRect = RECT2(10, 61, 10 + 7, 61 + 7);
			GAME_ENGINE->DrawBitmap(hudBmpPtr, left, top, srcRect);

			// NUMBER OF STARS
			left += 22;
			top -= 7;
			HUD::PaintSeveralDigitLargeNumber(left, top, m_FinalExtraScore.m_BonusScoreShowing);
		}
	}
	
	if (m_FramesShowingEndScreen < backToClearTransition)
	{
		if (m_FramesShowingEndScreen >= fadeFromBlackTransition)
		{
			// Fade from black to clear
			int alpha = int(255 - (double(m_FramesShowingEndScreen - fadeFromBlackTransition) / double(fadeTransition)) * 255);
			GAME_ENGINE->SetColor(COLOR(0, 0, 0, alpha));
			GAME_ENGINE->FillRect(0, 0, Game::WIDTH, Game::HEIGHT);
		}
	}
	else
	{
		if (m_FramesShowingEndScreen == backToClearTransition)
		{
			SoundManager::PlaySoundEffect(SoundManager::Sound::OUTRO_CIRCLE_TRANSITION);
		}
		else if (m_FramesShowingEndScreen < resetGameTransition)
		{
			double percentage = (resetGameTransition - m_FramesShowingEndScreen) / double(resetGameTransition - backToClearTransition);
			double innerCircleRadius = percentage * Game::WIDTH;
			DOUBLE2 playerPosScreenSpace = m_CameraPtr->GetViewMatrix().TransformPoint(m_PlayerPtr->GetPosition());
			PaintEnclosingCircle(playerPosScreenSpace, innerCircleRadius);
		}
		else
		{
			Reset();
			return;
		}
	}
}

// Paints everything black except a circle with the specified radius at the specified pos
void Level::PaintEnclosingCircle(DOUBLE2 circleCenter, double innerCircleRadius)
{
	GAME_ENGINE->SetColor(COLOR(0, 0, 0));

	// Fill the largest four rectangles you can, and then draw several circles (just outlines)
	double circleLeft = circleCenter.x - innerCircleRadius;
	double circleRight = circleCenter.x + innerCircleRadius;
	double circleTop = circleCenter.y - innerCircleRadius;
	double circleBottom = circleCenter.y + innerCircleRadius;
	if (circleLeft > 1)
	{
		GAME_ENGINE->FillRect(0, 0, circleLeft, Game::HEIGHT);
	}
	if (circleRight < Game::WIDTH - 1)
	{
		GAME_ENGINE->FillRect(circleRight, 0, Game::WIDTH, Game::HEIGHT);
	}
	if (circleTop > 1)
	{
		GAME_ENGINE->FillRect(0, 0, Game::WIDTH, circleTop);
	}
	if (circleBottom < Game::HEIGHT - 1)
	{
		GAME_ENGINE->FillRect(0, circleBottom, Game::WIDTH, Game::HEIGHT);
	}

	int numOfCircles = 15;
	double radius = innerCircleRadius;
	for (int i = 0; i < numOfCircles; ++i)
	{
		GAME_ENGINE->DrawEllipse(circleCenter, radius, radius, 7);
		radius += 5;
	}

}

void Level::PaintHUD()
{
	int playerLives = m_PlayerPtr->GetLives();
	int playerDragonCoins = m_PlayerPtr->GetDragonCoinsCollected();
	int playerStars = m_PlayerPtr->GetStarsCollected();
	int playerCoins = m_PlayerPtr->GetCoinsCollected();
	int playerScore = m_PlayerPtr->GetScore();
	Item::Type playerExtraItemType = m_PlayerPtr->GetExtraItemType();

	// NOTE: 1 second in game is 2/3 of a real life second!
	m_TimeRemaining = TOTAL_TIME - (int(m_SecondsElapsed * 1.5));
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
	HUD::PaintSeveralDigitLargeNumber(x, y, playerStars);

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
				m_PlayerPtr->GetLinearVelocity().y <= 0)
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
				Item* thisItem = (Item*)actThisPtr->GetUserPointer();
				if (thisItem->GetType() == Item::Type::ROTATING_BLOCK)
				{
					// NOTE: Only let p-switches fall through if the rotating block is spinning
					if (((RotatingBlock*)thisItem)->IsRotating())
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
			bool shellIsBeingHeld = m_PlayerPtr->GetHeldItemPtr() == koopaShellPtr;

			if (actThisPtr->GetUserData() != int(ActorId::ENEMY) &&
				shellIsBeingHeld ||
				(actOtherPtr->GetLinearVelocity().y < 0.0))
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
						m_PlayerPtr->Bounce();
					}
					else
					{
						m_PlayerPtr->TakeDamage();
					}
				}
				else if (m_PlayerPtr->IsRunning() && m_PlayerPtr->IsHoldingItem() == false)
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

				if (m_PlayerPtr->IsRunning() && m_PlayerPtr->IsHoldingItem() == false)
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
						m_PlayerPtr->Bounce();
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
						// TODO: Rename this method because not only shell hits cause this behaviour
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
							m_PlayerPtr->Bounce();
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
					m_PlayerPtr->Bounce(horizontalDir);
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
				const DOUBLE2 point2 = grabBlockPtr->GetPosition() + DOUBLE2((Block::WIDTH / 2) * grabBlockDir, 0);
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
		}
	} break;
	case int(ActorId::YOSHI_TOUNGE):
	{
		switch (actThisPtr->GetUserData())
		{
		case int(ActorId::ENEMY):
		{
			Enemy* enemyPtr = (Enemy*)actThisPtr->GetUserPointer();
			m_YoshiPtr->EatEnemy(enemyPtr);
		} break;
		case int(ActorId::ITEM):
		{
			Item* itemPtr = (Item*)actThisPtr->GetUserPointer();
			m_YoshiPtr->EatItem(itemPtr);
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
		SoundManager::SetSongPaused(m_BackgroundSong, m_Paused);
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

bool Level::IsYoshiAlive() const
{
	return m_YoshiPtr != nullptr;
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
