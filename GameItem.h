#pragma once

#include "Game.h"
#include "Entity.h"
#include "Enumerations.h"
#include "Level.h"

class LevelData;

struct Platform
{
	// NOTE: All platforms have the same height, this prevents too thin or thick 
	// of platforms from accidentally being made
	Platform(double left, double top, double right);
	virtual ~Platform();
	void AddContactListener(ContactListener* listener);
	double GetWidth();
	double GetHeight();
	bool Raycast(DOUBLE2 point1, DOUBLE2 point2, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef);

	static const int HEIGHT = 6;

private:
	PhysicsActor* m_ActPtr = nullptr;
	double m_Width;

	// TODO: Add angled platforms
	// TODO: Add moving platforms
};

struct Pipe
{
	static const std::string ColourNames[];

	Pipe(DOUBLE2 topLeft, DOUBLE2 bottomRight, bool canAccess);
	virtual ~Pipe();
	void AddContactListener(ContactListener* listener);
	bool Raycast(DOUBLE2 point1, DOUBLE2 point2, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef);

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

	Item(DOUBLE2 topLeft, TYPE type, Level* levelPtr, 
		BodyType bodyType = BodyType::STATIC, int width = 16, int height = 16);
	virtual ~Item();

	virtual void Tick(double deltaTime) = 0;
	virtual void Paint() = 0;

	TYPE GetType();

protected:
	const int WIDTH;
	const int HEIGHT;
	RECT2 m_Bounds;
private:
	TYPE m_Type;

	// TODO: Add velocity member? (And an isStatic field? or just a vel of 0)
};

struct Coin : public Item
{
	Coin(DOUBLE2 topLeft, Level* levelPtr, int life = -1, Item::TYPE type = Item::TYPE::COIN, DOUBLE2 size = DOUBLE2(WIDTH, HEIGHT));

	void Tick(double deltaTime);
	void Paint();
	int GetLifeRemaining();
	void GenerateParticles(); // Called when this coin is collected

	static const int LIFETIME = 25;

protected:
	static const int WIDTH = 16;
	static const int HEIGHT = 16;

private:

	int m_Life;
};

struct DragonCoin : public Coin
{
	DragonCoin(DOUBLE2 topLeft, Level* levelPtr);
	void Tick(double deltaTime);
	void Paint();
	void GenerateParticles();

	static const int WIDTH = 16;
	static const int HEIGHT = 16;
};

struct PSwitch : public Item
{
	PSwitch(DOUBLE2 topLeft, COLOUR colour, Level* levelPtr);

	void Tick(double deltaTime);
	void Paint();

private:
	COLOUR m_Colour;
	bool m_IsPressed = false;
};

struct OneUpMushroom : public Item
{
	OneUpMushroom(DOUBLE2 topLeft, Level* levelPtr);
	void Tick(double deltaTime);
	void Paint();
};
struct ThreeUpMoon : public Item
{
	ThreeUpMoon(DOUBLE2 topLeft, Level* levelPtr);
	void Tick(double deltaTime);
	void Paint();
};
struct SuperMushroom : public Item
{
	// horizontalDir is either 1 or -1, signifying which direction to move in (1:right, -1:left)
	SuperMushroom(DOUBLE2 topLeft, Level* levelPtr, int directionFacing = 1, bool isStatic = false);
	void Tick(double deltaTime);
	void Paint();
private:
	static const int HORIZONTAL_VEL = 80;
	int m_DirFacing;
	bool m_IsStatic;
	int m_CollisionBits = Level::COLLIDE_WITH_LEVEL | Level::COLLIDE_WITH_PLAYER;
};
struct FireFlower : public Item
{
	FireFlower(DOUBLE2 topLeft, Level* levelPtr);
	void Tick(double deltaTime);
	void Paint();
};
struct CapeFeather : public Item
{
	CapeFeather(DOUBLE2 topLeft, Level* levelPtr);
	void Tick(double deltaTime);
	void Paint();
};

struct Block : public Item
{
	static const int WIDTH = 16;
	static const int HEIGHT = 16;

	Block(DOUBLE2 topLeft, Item::TYPE type, Level* levelPtr);
	virtual ~Block();
	virtual void Tick(double deltaTime) = 0;
	virtual void Paint() = 0;
	virtual void Hit(Level* levelPtr) = 0;
};
struct PrizeBlock : public Block
{
	// LATER: Find out if there are any blocks spawned as used blocks ever in the real game
	PrizeBlock(DOUBLE2 topLeft, Level* levelPtr);
	void Tick(double deltaTime);
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
	ExclamationMarkBlock(DOUBLE2 topLeft, COLOUR colour, bool isSolid, Level* levelPtr);
	void Tick(double deltaTime);
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
	RotatingBlock(DOUBLE2 topLeft, Level* levelPtr);
	void Tick(double deltaTime);
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
	MessageBlock(DOUBLE2 topLeft, String bmpFilePath, Level* levelPtr);
	virtual ~MessageBlock();
	void Tick(double deltaTime);
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
