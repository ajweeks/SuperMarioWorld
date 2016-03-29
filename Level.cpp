#include "stdafx.h"

#include "Game.h"
#include "Level.h"
#include "Camera.h"
#include "LevelData.h"
#include "SpriteSheetManager.h"

#define GAME_ENGINE (GameEngine::GetSingleton())

Level::Level()
{
	// TODO: Make player extend Entity
	m_PlayerPtr = new Player();

	m_ActLevelPtr = new PhysicsActor(DOUBLE2(0, 0), 0, BodyType::STATIC);
	m_ActLevelPtr->AddSVGFixture(String("Resources/levels/01/Level01hitx2.svg"), 0.0);
	m_ActLevelPtr->AddContactListener(this);
	m_ActLevelPtr->SetUserData(int(ActorId::LEVEL));

	SpriteSheetManager::levelOneForeground->SetTransparencyColor(COLOR(255,0,255));

	m_Width = SpriteSheetManager::levelOneForeground->GetWidth();
	m_Height = SpriteSheetManager::levelOneForeground->GetHeight();

	m_Camera = new Camera(GAME_ENGINE->GetWidth(), GAME_ENGINE->GetHeight(), this);

	Reset();

	ReadLevelData(1);
}

Level::~Level()
{
	delete m_ActLevelPtr;
	delete m_PlayerPtr;
	delete m_Camera;
}

void Level::ReadLevelData(int levelIndex)
{
	m_LevelDataPtr = LevelData::GetLevel(levelIndex);

	std::vector<Platform*> platformsData = m_LevelDataPtr->GetPlatforms();
	std::vector<Pipe*> pipesData = m_LevelDataPtr->GetPipes();
	std::vector<Item*> itemsData = m_LevelDataPtr->GetItems();
	
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
}

void Level::Reset()
{
	m_PlayerPtr->Reset();
	m_Camera->Reset();

	m_LevelDataPtr->RegenerateLevel(1);
	ReadLevelData(1);

	m_TotalTime = 400; // This changes for every level, TODO: Put this info in the level save
	m_SecondsElapsed = 0.0;
}

void Level::Tick(double deltaTime)
{
	m_SecondsElapsed += (deltaTime);

	if (m_NewCoinPos != DOUBLE2())
	{
		// There is a coin we need to generate
		Coin* newCoin = new Coin(m_NewCoinPos, 15);
		m_LevelDataPtr->AddItem(newCoin);

		// TODO: Test that this works in all cases, it may 
		//be slightly janky since the player isn't actually touching the coin
		m_PlayerPtr->OnItemPickup(newCoin); 
		m_NewCoinPos = DOUBLE2();
	}
	if (m_ItemToBeRemoved != nullptr)
	{
		// The player collided with a coin during begin-contact, we need to remove it now
		m_LevelDataPtr->RemoveItem(m_ItemToBeRemoved);
		m_ItemToBeRemoved = nullptr;
	}

	m_PlayerPtr->Tick(deltaTime, this);

	m_LevelDataPtr->TickItems(deltaTime, this);
}

void Level::Paint()
{
#if 0
	DEBUGPaintZoomedOut();
	return;
#endif

	MATRIX3X2 matCameraView = m_Camera->GetViewMatrix(m_PlayerPtr, this);
	GAME_ENGINE->SetViewMatrix(matCameraView);

	int bgWidth = SpriteSheetManager::levelOneBackground->GetWidth();
	double cameraX = -matCameraView.orig.x;
	double parallax = (1.0 - 0.65); // 65% of the speed of the camera
	int xo = int(cameraX*parallax);
	xo += (int(cameraX - xo) / bgWidth) * bgWidth;

	GAME_ENGINE->DrawBitmap(SpriteSheetManager::levelOneBackground, DOUBLE2(xo, 0));
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::levelOneBackground, DOUBLE2(xo + bgWidth, 0));

	GAME_ENGINE->DrawBitmap(SpriteSheetManager::levelOneForeground);

	m_LevelDataPtr->PaintItems();

	m_PlayerPtr->Paint();
	
	GAME_ENGINE->SetViewMatrix(MATRIX3X2::CreateIdentityMatrix());

	PaintHUD();

#if 0
	m_Camera->DEBUGPaint();
#endif

#if 0
	GAME_ENGINE->SetColor(COLOR(0, 0, 0));
	GAME_ENGINE->SetFont(Game::Font16Ptr);
	GAME_ENGINE->DrawString(String("Player pos: ") + m_PlayerPtr->GetPosition().ToString(), 10, 10);
	GAME_ENGINE->DrawString(String("Player vel: ") + m_PlayerPtr->GetLinearVelocity().ToString(), 10, 25);
	GAME_ENGINE->DrawString(String("Player onGround: ") + String(m_PlayerPtr->IsOnGround() ? "true" : "false"), 10, 40);
#endif

	GAME_ENGINE->SetViewMatrix(matCameraView);
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

	int timeRemaining = m_TotalTime - int(m_SecondsElapsed);

	const int SCALE = 2;

	int x = 31;
	int y = 30;
	RECT2 srcRect;

	// LATER: Add luigi here when he's playing
	// MARIO
	srcRect = RECT2(1 * SCALE, 1 * SCALE, 41 * SCALE, 9 * SCALE);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::hud, x, y, srcRect);
	
	// X
	x += 18;
	y += 17;
	srcRect = RECT2(10 * SCALE, 61 * SCALE, 10 * SCALE + 7 * SCALE, 61 * SCALE + 7 * SCALE);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::hud, x, y, srcRect);

	// LIVES
	x += 30;
	srcRect = GetSmallSingleNumberSrcRect(playerLives, false);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::hud, x, y, srcRect);

	// DRAGON COINS
	x += 48;
	y = 30;
	for (size_t i = 0; i < playerDragonCoins; ++i)
	{
		srcRect = RECT2(1 * SCALE, 60 * SCALE, 1 * SCALE + 8 * SCALE, 60 * SCALE + 8 * SCALE);
		GAME_ENGINE->DrawBitmap(SpriteSheetManager::hud, x, y, srcRect);
		x += 8 * SCALE;
	}

	// RED STAR
	y += 16;
	x = 140;
	srcRect = RECT2(19 * SCALE, 60 * SCALE, 19 * SCALE + 8 * SCALE, 60 * SCALE + 8 * SCALE);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::hud, x, y, srcRect);

	// X
	x += 20;
	y += 3;
	srcRect = RECT2(10 * SCALE, 61 * SCALE, 10 * SCALE + 7 * SCALE, 61 * SCALE + 7 * SCALE);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::hud, x, y, srcRect);

	// NUMBER OF STARS
	x += 48;
	y -= 15;
	PaintSeveralDigitLargeNumber(x, y, playerStars);

	// ITEM BOX
	x += 20;
	y -= 15;
	srcRect = RECT2(36 * SCALE, 52 * SCALE, 36 * SCALE + 28 * SCALE, 52 * SCALE + 28 * SCALE);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::hud, x, y, srcRect);

	// TIME
	x += 75;
	y = 30;
	srcRect = RECT2(1 * SCALE, 52 * SCALE, 1 * SCALE + 24 * SCALE, 52 * SCALE + 7 * SCALE);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::hud, x, y, srcRect);

	// TIME VALUE
	y += 19;
	x += 32;
	PaintSeveralDigitNumber(x, y, timeRemaining, true);

	// COIN LABEL
	x += 66;
	y = 30;
	srcRect = RECT2(1 * SCALE, 60 * SCALE, 1 * SCALE + 16 * SCALE, 60 * SCALE + 8 * SCALE);
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::hud, x, y, srcRect);

	// COINS
	x += 60;
	PaintSeveralDigitNumber(x, y, playerCoins, false);

	// SCORE
	y += 17;
	PaintSeveralDigitNumber(x, y, playerScore, false);
}

// TODO: Move all SCALE references to one global constant in Game.h
// TODO: Just set a world matrix to print these instead of passing numbers
// NOTE: The x coordinate specifies the placement of the right-most digit
void Level::PaintSeveralDigitNumber(int x, int y, int number, bool yellow)
{
	int SCALE = 2;

	number = abs(number);

	do {
		int digit = number % 10;
		RECT2 srcRect = GetSmallSingleNumberSrcRect(digit, yellow);
		GAME_ENGINE->DrawBitmap(SpriteSheetManager::hud, x, y, srcRect);

		x -= 8 * SCALE;
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
RECT2 Level::GetSmallSingleNumberSrcRect(int number, bool yellow)
{
	assert(number >= 0 && number <= 9);

	RECT2 result;

	const int SCALE = 2;

	int numberWidth = 8 * SCALE;
	int numberHeight = 7 * SCALE;
	int xo = 0 + numberWidth * number;
	int yo = 34 * SCALE;

	if (yellow) yo += 10 * SCALE;

	result = RECT2(xo, yo, xo + numberWidth, yo + numberHeight);

	return result;
}

void Level::PaintSeveralDigitLargeNumber(int x, int y, int number)
{
	int SCALE = 2;

	number = abs(number);

	do {
		int digit = number % 10;
		RECT2 srcRect = GetLargeSingleNumberSrcRect(digit);
		GAME_ENGINE->DrawBitmap(SpriteSheetManager::hud, x, y, srcRect);

		x -= 8 * SCALE;
		number /= 10;
	} while (number > 0);
}

RECT2 Level::GetLargeSingleNumberSrcRect(int number)
{
	assert(number >= 0 && number <= 9);

	RECT2 result;

	const int SCALE = 2;

	int numberWidth = 8 * SCALE;
	int numberHeight = 14 * SCALE;
	int xo = 0 + numberWidth * number;
	int yo = 19 * SCALE;

	result = RECT2(xo, yo, xo + numberWidth, yo + numberHeight);

	return result;
}

 void Level::DEBUGPaintZoomedOut()
{
	MATRIX3X2 matCameraView = m_Camera->GetViewMatrix(m_PlayerPtr, this);
	MATRIX3X2 matZoom = MATRIX3X2::CreateScalingMatrix(0.25);
	MATRIX3X2 matCenterTranslate = MATRIX3X2::CreateTranslationMatrix(150, 160);
	GAME_ENGINE->SetViewMatrix(matCameraView * matZoom * matCenterTranslate);

	int bgWidth = SpriteSheetManager::levelOneBackground->GetWidth();
	double cameraX = -matCameraView.orig.x;

	double parallax = (1.0 - 0.65); // 65% of the speed of the camera
	int xo = int(cameraX*parallax);
	xo += (int(cameraX - xo) / bgWidth) * bgWidth;

	GAME_ENGINE->DrawBitmap(SpriteSheetManager::levelOneBackground, DOUBLE2(xo, 0));
	GAME_ENGINE->DrawBitmap(SpriteSheetManager::levelOneBackground, DOUBLE2(xo + bgWidth, 0));

	int nearestBoundary = int(cameraX - (int(cameraX) % bgWidth));
	GAME_ENGINE->SetColor(COLOR(200, 20, 20));
	GAME_ENGINE->DrawLine(nearestBoundary, 0, nearestBoundary, 5000, 5);
	GAME_ENGINE->DrawLine(nearestBoundary + bgWidth, 0, nearestBoundary + bgWidth, 5000, 5);

	GAME_ENGINE->SetColor(COLOR(20, 20, 20));
	GAME_ENGINE->DrawLine(xo + bgWidth, 0, xo + bgWidth, 5000, 5);

	GAME_ENGINE->DrawBitmap(SpriteSheetManager::levelOneForeground);

	m_PlayerPtr->Paint();

	// Draw camera outline
	GAME_ENGINE->SetWorldMatrix(matCameraView.Inverse());
	GAME_ENGINE->SetColor(COLOR(20, 20, 200));
	GAME_ENGINE->DrawRect(5, 5, GAME_ENGINE->GetWidth() - 5, GAME_ENGINE->GetHeight() - 5, 5);
	GAME_ENGINE->SetWorldMatrix(MATRIX3X2::CreateIdentityMatrix());

	GAME_ENGINE->SetViewMatrix(MATRIX3X2::CreateIdentityMatrix());

	GAME_ENGINE->SetColor(COLOR(0, 0, 0));
	GAME_ENGINE->DrawString(String("Player pos: ") + m_PlayerPtr->GetPosition().ToString(), 10, 10);

	GAME_ENGINE->SetViewMatrix(matCameraView);
}

bool Level::IsOnGround(PhysicsActor *otherActPtr)
{
	return m_IsPlayerOnGround;
	/*bool onGround = m_ActLevelPtr->IsOverlapping(otherActPtr);
	if (onGround) return true;

	bool onPlatform = false;
	for (size_t i = 0; i < m_PlatformsPtrArr.size(); ++i)
	{
		if (m_PlatformsPtrArr[i]->IsOverlapping(otherActPtr))
		{
			onPlatform = true;
			break;
		}
	}
	if (onPlatform) return true;

	bool onPipe = false;
	for (size_t i = 0; i < m_PipesPtrArr.size(); ++i)
	{
		if (m_PipesPtrArr[i]->IsOverlapping(otherActPtr))
		{
			onPipe = true;
			break;
		}
	}
	if (onPipe) return true;

	return false;*/
}

void Level::PreSolve(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr, bool & enableContactRef)
{
	if (actOtherPtr->GetUserData() == int(ActorId::PLAYER))
	{
		bool rising = actOtherPtr->GetLinearVelocity().y < 0;
		bool belowThis = (actOtherPtr->GetPosition().y + Player::HEIGHT / 2) > actThisPtr->GetPosition().y;

		switch (actThisPtr->GetUserData())
		{
		case int(ActorId::PLATFORM):
		{
			if (rising || belowThis)
			{
				enableContactRef = false;
			}
		} break;
		case int(ActorId::LEVEL):
		case int(ActorId::PIPE):
		{
			if (rising)
			{
				actOtherPtr->SetLinearVelocity(DOUBLE2(0, actOtherPtr->GetLinearVelocity().y));
				enableContactRef = false;
			}
			else
			{

			}
		} break;
		case int(ActorId::ITEM) :
		{
			Item* item = (Item*)actThisPtr->GetUserPointer();
			switch (item->GetType())
			{
			case Item::TYPE::PRIZE_BLOCK:
			{
				// They be bonkin their head on us
				if (actOtherPtr->GetLinearVelocity().y < 0.0 && 
					actOtherPtr->GetPosition().y > actThisPtr->GetPosition().y + Block::WIDTH/2)
				{
					m_NewCoinPos = ((PrizeBlock*)item)->Hit();
				}
			} break;
			}
		} break;
		}
	}
}

void Level::BeginContact(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr)
{
	if (actOtherPtr->GetUserData() == int(ActorId::PLAYER))
	{
		if (actThisPtr->GetUserData() == int(ActorId::PLATFORM) ||
			actThisPtr->GetUserData() == int(ActorId::LEVEL))
		{
			m_IsPlayerOnGround = true;
		}
		else if (actThisPtr->GetUserData() == int(ActorId::ITEM))
		{
			Item* item = (Item*)actThisPtr->GetUserPointer();
			switch (item->GetType())
			{
			case Item::TYPE::COIN:
			{
				assert(m_ItemToBeRemoved == nullptr);

				((Player*)actOtherPtr->GetUserPointer())->OnItemPickup(item);
				m_ItemToBeRemoved = item;
			} break;
			case Item::TYPE::DRAGON_COIN:
			{
				assert(m_ItemToBeRemoved == nullptr);

				((Player*)actOtherPtr->GetUserPointer())->OnItemPickup(item);
				m_ItemToBeRemoved = item;
			} break;
			}
		}
	}
}

void Level::EndContact(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr)
{
	if (actOtherPtr->GetUserData() == int(ActorId::PLAYER))
	{
		m_IsPlayerOnGround = false;
	}
}

void Level::TogglePaused(bool paused)
{
	m_PlayerPtr->TogglePaused(paused);
	m_LevelDataPtr->TogglePaused(paused);
}

double Level::GetWidth()
{
	return m_Width;
}

double Level::GetHeight()
{
	return m_Height;
}
