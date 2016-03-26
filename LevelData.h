#pragma once
#include "Enumerations.h"
#include "SpriteSheetManager.h"
#include "SpriteSheet.h"
#include "Entity.h"

#define GAME_ENGINE (GameEngine::GetSingleton())

struct Platform
{
	Platform(DOUBLE2 topLeft, DOUBLE2 bottomRight)
	{
		m_Bounds = RECT2(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
		double width = bottomRight.x - topLeft.x;
		double height = bottomRight.y - topLeft.y;
		m_ActPtr = new PhysicsActor(topLeft + DOUBLE2(width/2, height/2), 0, BodyType::STATIC);
		m_ActPtr->AddBoxFixture(width, height, 0.0);
		m_ActPtr->SetUserData(int(ActorId::PLATFORM));
	}
	virtual ~Platform()
	{
		delete m_ActPtr;
	}
	void AddContactListener(ContactListener* listener)
	{
		m_ActPtr->AddContactListener(listener);
	}
	// TODO: Add angled platforms

	PhysicsActor* m_ActPtr = nullptr;
	RECT2 m_Bounds;
};
// TODO: Add moving platforms

struct Pipe
{
	enum class Colour
	{
		GREEN, YELLOW, BLUE, ORANGE, GREY
	};

	static const std::string ColourNames[];

	Pipe(DOUBLE2 topLeft, DOUBLE2 bottomRight, Colour colour, bool canAccess) :
		m_Colour(colour), m_CanAccess(canAccess)
	{
		m_Bounds = RECT2(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
		double width = bottomRight.x - topLeft.x;
		double height = bottomRight.y - topLeft.y;
		m_ActPtr = new PhysicsActor(topLeft + DOUBLE2(width / 2, height / 2), 0, BodyType::STATIC);
		m_ActPtr->AddBoxFixture(width, height, 0.0);
		m_ActPtr->SetUserData(int(ActorId::PIPE));
	}
	virtual ~Pipe()
	{
		delete m_ActPtr;
	}
	void AddContactListener(ContactListener* listener)
	{
		m_ActPtr->AddContactListener(listener);
	}
	// TODO: Add angled pipes

	PhysicsActor* m_ActPtr = nullptr;
	RECT2 m_Bounds;
	Colour m_Colour;
	// If true mario can enter this pipe
	bool m_CanAccess; 
	// Color ?
};

struct Item : public Entity
{
	enum class TYPE
	{
		COIN, DRAGON_COIN, BERRY, KEY, KEYHOLE, SWITCH_BLOCK, 
		ONE_UP_MUSHROOM, THREE_UP_MOON,
		SUPER_MUSHROOM, FIRE_FLOWER, CAPE_FEATHER, POWER_BALLOON, STAR,
		PRIZE_BLOCK, USED_BLOCK, MESSAGE_BLOCK, ROTATING_BLOCK, EXCLAMATION_MARK_BLOCK
	};

	Item(DOUBLE2 topLeft, DOUBLE2 bottomRight) : 
		Entity(topLeft + DOUBLE2((bottomRight.x - topLeft.x) / 2, (bottomRight.y - topLeft.y) / 2), 
			SpriteSheetManager::generalTiles,
			BodyType::KINEMATIC)
	{
		double width = bottomRight.x - topLeft.x;
		double height = bottomRight.y - topLeft.y;
		m_ActPtr->AddBoxFixture(width, height, 0.0);
		m_ActPtr->SetUserData(int(ActorId::ITEM));
	}
	virtual ~Item()
	{
	}
	void AddContactListener(ContactListener* listener)
	{
		m_ActPtr->AddContactListener(listener);
	}
	void SetUserPointer(void* userPointer)
	{
		if (typeid(userPointer) != typeid(Item*))
		{
			GAME_ENGINE->MessageBox(String("INVALID USER POINTER PASSED TO SetUserPointer (LevelData.h)"));
			return;
		}
		m_ActPtr->SetUserPointer(userPointer);
	}
	Item* GetUserPointer()
	{
		return (Item*)(m_ActPtr->GetUserPointer());
	}
	virtual void Tick(double deltaTime, Level* levelPtr) = 0;
	virtual void Paint() = 0;

	// TODO: Add velocity member? (And an isStatic field? or just a vel of 0)

	ANIMATION_INFO m_AnimInfo;
	TYPE m_Type;
	RECT2 m_Bounds;
};

struct Coin : public Item
{
	static const int WIDTH = 16;
	static const int HEIGHT = 16;

	Coin(DOUBLE2 centerPos) :
		Item(centerPos, centerPos + DOUBLE2(WIDTH, HEIGHT))
	{
		m_Type = TYPE::COIN;
	}
	// TODO: Make this virtual?
	void Tick(double deltaTime, Level* levelPtr)
	{
		m_AnimInfo.Tick(deltaTime);
		m_AnimInfo.frameNumber %= 4;
	}
	void Paint()
	{
		double srcX = 0 + m_AnimInfo.frameNumber * WIDTH;
		double srcY = 0;
		RECT2 srcRect = RECT2(srcX, srcY, srcX + WIDTH, srcY + HEIGHT);
		double left = m_ActPtr->GetPosition().x - WIDTH/2;
		double top = m_ActPtr->GetPosition().y - HEIGHT/2;
		SpriteSheetManager::generalTiles->Paint(left, top, srcRect);
	}
};
struct DragonCoin : public Coin
{
	DragonCoin(DOUBLE2 centerPos) :
		Coin(centerPos)
	{
		m_Type = TYPE::DRAGON_COIN;
	}
	void Tick(double deltaTime, Level* levelPtr)
	{
		m_AnimInfo.Tick(deltaTime);
		m_AnimInfo.frameNumber %= 6;
	}
	void Paint()
	{
		double srcX = 0 + m_AnimInfo.frameNumber * WIDTH;
		double srcY = 2 * HEIGHT;
		RECT2 srcRect = RECT2(srcX, srcY, srcX + WIDTH, srcY + HEIGHT * 2);
		double left = m_ActPtr->GetPosition().x - WIDTH/2;
		double top = m_ActPtr->GetPosition().y - HEIGHT/2;
		SpriteSheetManager::generalTiles->Paint(left, top, srcRect);
	}
};

struct Block : public Item
{
	static const int WIDTH = 16 * 2;
	static const int HEIGHT = 16 * 2;

	Block(DOUBLE2 topLeft) :
		Item(topLeft, topLeft + DOUBLE2(WIDTH, HEIGHT))
	{
	}
	virtual void Tick(double deltaTime, Level* levelPtr) {};
	virtual void Paint() {};
};
struct PrizeBlock : public Block
{
	// LATER: Find out if there are any blocks spawned as used blocks ever
	PrizeBlock(DOUBLE2 topLeft) :
		Block(topLeft)
	{
		m_Type = TYPE::PRIZE_BLOCK;
	}
	void Tick(double deltaTime, Level* levelPtr)
	{
		m_AnimInfo.Tick(deltaTime);
		m_AnimInfo.frameNumber %= 4;
	}
	void Paint()
	{
		double srcX = 0 + m_AnimInfo.frameNumber * WIDTH;
		double srcY = 4 * HEIGHT;
		RECT2 srcRect = RECT2(srcX, srcY, srcX + WIDTH, srcY + HEIGHT);
		double left = m_ActPtr->GetPosition().x;
		double top = m_ActPtr->GetPosition().y;
		SpriteSheetManager::generalTiles->Paint(left, top, srcRect);
	}

	bool m_IsUsed = false;
};
struct ExclamationMarkBlock : public Block
{
	enum class COLOUR
	{
		YELLOW, GREEN, RED, BLUE
	};

	ExclamationMarkBlock(DOUBLE2 topLeft, COLOUR colour) :
		Block(topLeft), m_Colour(colour)
	{
		m_Type = TYPE::EXCLAMATION_MARK_BLOCK;
		m_AnimInfo.frameNumber = 0;
	}
	void Tick(double deltaTime, Level* levelPtr)
	{

	}
	void Paint()
	{
		double srcX = 1 * WIDTH;
		double srcY = 10 * HEIGHT;
		RECT2 srcRect = RECT2(srcX, srcY, srcX + WIDTH, srcY + HEIGHT);
		double left = m_ActPtr->GetPosition().x;
		double top = m_ActPtr->GetPosition().y;
		SpriteSheetManager::generalTiles->Paint(left, top, srcRect);
	}

	COLOUR m_Colour;
};
struct RotatingBlock : public Block
{
	RotatingBlock(DOUBLE2 topLeft) :
		Block(topLeft)
	{
		m_Type = TYPE::ROTATING_BLOCK;
	}
	void Tick(double deltaTime, Level* levelPtr)
	{
		if (m_IsRotating)
		{
			m_AnimInfo.Tick(deltaTime);
			m_AnimInfo.frameNumber %= 4;
			if (m_AnimInfo.frameNumber == 3)
			{
				m_Rotations++;
				if (m_Rotations > MAX_ROTATIONS)
				{
					m_Rotations = 0;
					m_IsRotating = false;
				}
			}
		}
	}
	void Paint()
	{
		double srcX = 0 + m_AnimInfo.frameNumber * WIDTH;
		double srcY = 5 * HEIGHT;
		RECT2 srcRect = RECT2(srcX, srcY, srcX + WIDTH, srcY + HEIGHT);
		double left = m_ActPtr->GetPosition().x;
		double top = m_ActPtr->GetPosition().y;
		SpriteSheetManager::generalTiles->Paint(left, top, srcRect);
	}
	static const int MAX_ROTATIONS = 8;

	int m_Rotations = -1;
	bool m_IsRotating;
};
struct MessageBlock : public Block
{
	MessageBlock(DOUBLE2 topLeft, String message) :
		Block(topLeft), m_Message(message)
	{
		m_Type = TYPE::MESSAGE_BLOCK;
		m_AnimInfo.frameNumber = 0;
	}
	void Paint()
	{
		double srcX = 5 * WIDTH;
		double srcY = 9 * HEIGHT;
		RECT2 srcRect = RECT2(srcX, srcY, srcX + WIDTH, srcY + HEIGHT);
		double left = m_ActPtr->GetPosition().x;
		double top = m_ActPtr->GetPosition().y;
		SpriteSheetManager::generalTiles->Paint(left, top, srcRect);
	}

private:
	String m_Message;
	ANIMATION_INFO m_AnimInfo;
};

/*

This class represents an entire level
It holds the following information:

- Platforms
- Pipes
- Enemies' spawns
- Items (Coins, Blocks, etc.)
- Midway gates & Goal gates

*/
class LevelData
{
public:
	virtual ~LevelData();

	LevelData& operator=(const LevelData&) = delete;
	LevelData(const LevelData&) = delete;

	static LevelData* GetLevel(int level);
	static void Unload();

	void PaintItems();
	void TickItems(double deltaTime, Level* levelPtr);

	std::vector<Platform*> GetPlatforms();
	std::vector<Pipe*> GetPipes();
	std::vector<Item*> GetItems();

private:
	LevelData(String platforms, String pipes, String items);

	static LevelData* GenerateLevel(int levelIndex);
	static LevelData* m_LevelOnePtr;

	DOUBLE2 StringToDOUBLE2(std::string double2String);

	std::vector<Platform*> m_PlatformsPtrArr;
	std::vector<Pipe*> m_PipesPtrArr;
	std::vector<Item*> m_ItemsPtrArr;
};
