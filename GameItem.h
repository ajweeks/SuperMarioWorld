#pragma once

#define GAME_ENGINE (GameEngine::GetSingleton())

#include "Entity.h"
#include "Enumerations.h"

class LevelData;

struct Platform
{
	Platform(DOUBLE2 topLeft, DOUBLE2 bottomRight);
	virtual ~Platform();
	void AddContactListener(ContactListener* listener);
	
private:
	PhysicsActor* m_ActPtr = nullptr;
	RECT2 m_Bounds;

	// TODO: Add angled platforms
	// TODO: Add moving platforms
};

struct Pipe
{
	static const std::string ColourNames[];

	Pipe(DOUBLE2 topLeft, DOUBLE2 bottomRight, bool canAccess);
	virtual ~Pipe();
	void AddContactListener(ContactListener* listener);

private:
	PhysicsActor* m_ActPtr = nullptr;
	RECT2 m_Bounds;
	// If true mario can enter this pipe
	bool m_CanAccess;

	// TODO: Add angled pipes
	// TODO: Add Color field?
};

struct Item : public Entity
{
	enum class TYPE
	{
		NONE,
		COIN, DRAGON_COIN, BERRY, KEY, KEYHOLE, P_SWITCH,
		ONE_UP_MUSHROOM, THREE_UP_MOON,
		SUPER_MUSHROOM, FIRE_FLOWER, CAPE_FEATHER, STAR, POWER_BALLOON,
		PRIZE_BLOCK, MESSAGE_BLOCK, ROTATING_BLOCK, EXCLAMATION_MARK_BLOCK
	};

	Item(DOUBLE2 topLeft, TYPE type, BodyType bodyType = BodyType::STATIC, int width = 16, int height = 16);

	void AddContactListener(ContactListener* listener);

	virtual bool Tick(double deltaTime, Level* levelPtr) = 0;
	virtual void Paint() = 0;

	void TogglePaused(bool paused);

	TYPE GetType();

protected:
	const int WIDTH;
	const int HEIGHT;
	ANIMATION_INFO m_AnimInfo;
	RECT2 m_Bounds;
private:
	TYPE m_Type;

	// TODO: Add velocity member? (And an isStatic field? or just a vel of 0)
};

struct Coin : public Item
{
	Coin(DOUBLE2 topLeft, int life = -1, Item::TYPE type = Item::TYPE::COIN, DOUBLE2 size = DOUBLE2(WIDTH, HEIGHT));

	virtual bool Tick(double deltaTime, Level* levelPtr);
	void Paint();

	static const int LIFETIME = 25;

protected:
	static const int WIDTH = 16;
	static const int HEIGHT = 16;

private:
	int m_Life;
};

struct DragonCoin : public Coin
{
	DragonCoin(DOUBLE2 topLeft);
	bool Tick(double deltaTime, Level* levelPtr);
	void Paint();

	static const int WIDTH = 16;
	static const int HEIGHT = 16;
};

struct PSwitch : public Item
{
	PSwitch(DOUBLE2 topLeft, COLOUR colour);

	bool Tick(double deltaTime, Level* levelPtr);
	void Paint();

private:
	COLOUR m_Colour;
	bool m_IsPressed = false;
};

struct OneUpMushroom : public Item
{
	OneUpMushroom(DOUBLE2 topLeft);
	bool Tick(double deltaTime, Level* levelPtr);
	void Paint();
};
struct ThreeUpMoon : public Item
{
	ThreeUpMoon(DOUBLE2 topLeft);
	bool Tick(double deltaTime, Level* levelPtr);
	void Paint();
};
struct SuperMushroom : public Item
{
	// horizontalDir is either 1 or -1, signifying which direction to move in (1:right, -1:left)
	SuperMushroom(DOUBLE2 topLeft, int horizontalDir = 1, bool isStatic = false);
	bool Tick(double deltaTime, Level* levelPtr);
	void Paint();
private:
	double m_HorizontalSpeed = 80;
	bool m_IsStatic;
};
struct FireFlower : public Item
{
	FireFlower(DOUBLE2 topLeft);
	bool Tick(double deltaTime, Level* levelPtr);
	void Paint();
};
struct CapeFeather : public Item
{
	CapeFeather(DOUBLE2 topLeft);
	bool Tick(double deltaTime, Level* levelPtr);
	void Paint();
};

struct Block : public Item
{
	static const int WIDTH = 16;
	static const int HEIGHT = 16;

	Block(DOUBLE2 topLeft, Item::TYPE type);
	virtual ~Block();
	virtual bool Tick(double deltaTime, Level* levelPtr) = 0;
	virtual void Paint() = 0;
	virtual void Hit(Level* levelPtr) = 0;
};
struct PrizeBlock : public Block
{
	// LATER: Find out if there are any blocks spawned as used blocks ever in the real game
	PrizeBlock(DOUBLE2 topLeft);
	bool Tick(double deltaTime, Level* levelPtr);
	void Paint();
	// NOTE: I think Box2D prevents you from adding physics actors during preSolve, therefore
	// we need to set a flag when the player hits a prize block and generate the coin in the next tick,
	// or at least after pre-solve
	void Hit(Level* levelPtr);

private:
	int m_CurrentFrameOfBumpAnimation = -1;
	int m_FramesOfBumpAnimation = 14;
	int m_yo = 0; // NOTE: Used for the bump animation
	bool m_IsUsed = false;
	bool m_ShouldSpawnCoin = false;
};
struct ExclamationMarkBlock : public Block
{
	ExclamationMarkBlock(DOUBLE2 topLeft, COLOUR colour, bool isSolid);
	bool Tick(double deltaTime, Level* levelPtr);
	void Paint();
	void SetSolid(bool solid);
	void Hit(Level* levelPtr);

private:
	COLOUR m_Colour;
	int m_CurrentFrameOfBumpAnimation = -1;
	int m_FramesOfBumpAnimation = 14;
	int m_yo = 0; // NOTE: Used for the bump animation
	// NOTE: This is set to true for every ! block when the player hits the
	// yellow switch palace switch block
	bool m_IsSolid = false;
	bool m_IsUsed = false;
	bool m_ShouldSpawnSuperMushroom = false;
};

struct RotatingBlock : public Block
{
	RotatingBlock(DOUBLE2 topLeft);
	bool Tick(double deltaTime, Level* levelPtr);
	void Paint();
	void Hit(Level* levelPtr);
	bool IsRotating();

	static const int MAX_ROTATIONS = 25;

private:
	int m_Rotations = -1;
	bool m_IsRotating = false;
};
struct MessageBlock : public Block
{
	MessageBlock(DOUBLE2 topLeft, String filePath);
	virtual ~MessageBlock();
	bool Tick(double deltaTime, Level* levelPtr);
	void Paint();
	void Hit(Level* levelPtr);

private:
	static int m_BitmapWidth;
	static int m_BitmapHeight;

	// LATER: Make message boxes render their text manually?
	//String m_Message;
	Bitmap* m_BmpPtr = nullptr;
	// NOTE: In the original game, after hitting a message block, the game doesn't pause for a second or so
	int m_FramesUntilPause = -1;
	static const int FRAMES_TO_WAIT = 12;
	// NOTE: Used to animate message popup, when == -1 there is no message showing
	// When != -1 the game is paused and the message is being painted (by the message block itself)
	int m_FramesOfIntroAnimation = -1;
	int m_FramesOfOutroAnimation = -1;
	static const int FRAMES_OF_ANIMATION;
};
