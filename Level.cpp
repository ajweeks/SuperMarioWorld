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
}

void Level::Tick(double deltaTime)
{
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

	GAME_ENGINE->SetColor(COLOR(0, 0, 0));
	GAME_ENGINE->SetFont(Game::Font16Ptr);
	GAME_ENGINE->DrawString(String("Player pos: ") + m_PlayerPtr->GetPosition().ToString(), 10, 10);
	GAME_ENGINE->DrawString(String("Player vel: ") + m_PlayerPtr->GetLinearVelocity().ToString(), 10, 25);
	GAME_ENGINE->DrawString(String("Player onGround: ") + String(m_PlayerPtr->IsOnGround() ? "true" : "false"), 10, 40);

	GAME_ENGINE->SetViewMatrix(matCameraView);
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
			switch (item->m_Type)
			{
			case Item::TYPE::PRIZE_BLOCK:
			{
				// They be bonkin their head on us
				if (actOtherPtr->GetLinearVelocity().y < 0.0 && 
					actOtherPtr->GetPosition().y > actThisPtr->GetPosition().y + Block::WIDTH/2)
				{
					((PrizeBlock*)item)->Hit(m_LevelDataPtr);
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
				m_IsPlayerOnGround = true;
				return;
		if (actThisPtr->GetUserData() == int(ActorId::PLATFORM))
		{
			if (actOtherPtr->GetLinearVelocity().y >= 0.0)
			{
				m_IsPlayerOnGround = true;
			}
		}
		else if (actOtherPtr->GetUserData() == int(ActorId::ITEM))
		{
			if (actOtherPtr->GetLinearVelocity().y >= 0.0)
			{
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
}

double Level::GetWidth()
{
	return m_Width;
}

double Level::GetHeight()
{
	return m_Height;
}