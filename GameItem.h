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
	enum class Colour
	{
		GREEN, YELLOW, BLUE, ORANGE, GREY
	};

	static const std::string ColourNames[];

	Pipe(DOUBLE2 topLeft, DOUBLE2 bottomRight, Colour colour, bool canAccess);
	virtual ~Pipe();
	void AddContactListener(ContactListener* listener);

private:
	PhysicsActor* m_ActPtr = nullptr;
	RECT2 m_Bounds;
	Colour m_Colour;
	// If true mario can enter this pipe
	bool m_CanAccess;

	// TODO: Add angled pipes
	// TODO: Add Color field?
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

	Item(DOUBLE2 topLeft, DOUBLE2 bottomRight);
	virtual ~Item();
	void AddContactListener(ContactListener* listener);

	virtual void Tick(double deltaTime, Level* levelPtr) = 0;
	virtual void Paint() = 0;

	ANIMATION_INFO m_AnimInfo;
	TYPE m_Type;
	RECT2 m_Bounds;

	// TODO: Add velocity member? (And an isStatic field? or just a vel of 0)
};

struct Coin : public Item
{
	static const int WIDTH = 16;
	static const int HEIGHT = 16;

	Coin(DOUBLE2 centerPos);
	virtual void Tick(double deltaTime, Level* levelPtr);
	void Paint();
};

struct DragonCoin : public Coin
{
	DragonCoin(DOUBLE2 centerPos);
	void Tick(double deltaTime, Level* levelPtr);
	void Paint();
};

struct Block : public Item
{
	static const int WIDTH = 16 * 2;
	static const int HEIGHT = 16 * 2;

	Block(DOUBLE2 topLeft);
	virtual void Tick(double deltaTime, Level* levelPtr) = 0;
	virtual void Paint() = 0;
};
struct PrizeBlock : public Block
{
	// LATER: Find out if there are any blocks spawned as used blocks ever in the real game
	PrizeBlock(DOUBLE2 topLeft);
	void Tick(double deltaTime, Level* levelPtr);
	void Paint();
	void Hit(LevelData* levelDataPtr); // NOTE: This is called when mario hits this block with his head

private:
	int m_CurrentFrameOfBumpAnimation = -1;
	int m_FramesOfBumpAnimation = 8;
	int m_yo = 0; // NOTE: Used for the bump animation
	bool m_IsUsed = false;
};
struct ExclamationMarkBlock : public Block
{
	enum class COLOUR
	{
		YELLOW, GREEN, RED, BLUE
	};

	ExclamationMarkBlock(DOUBLE2 topLeft, COLOUR colour);
	void Tick(double deltaTime, Level* levelPtr);
	void Paint();

private:
	COLOUR m_Colour;
};
struct RotatingBlock : public Block
{
	RotatingBlock(DOUBLE2 topLeft);
	void Tick(double deltaTime, Level* levelPtr);
	void Paint();
	static const int MAX_ROTATIONS = 8;

private:
	int m_Rotations = -1;
	bool m_IsRotating;
};
struct MessageBlock : public Block
{
	MessageBlock(DOUBLE2 topLeft, String message);
	void Paint();

private:
	String m_Message;
};
