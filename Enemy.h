#pragma once

#include "Entity.h"
#include "Enumerations.h"
#include "SMWTimer.h"

class Enemy : public Entity
{
public:
	enum class Type
	{
		NONE, KOOPA_TROOPA, CHARGIN_CHUCK, PIRHANA_PLANT, MONTY_MOLE
	};

	Enemy(Type type, DOUBLE2& posRef, double width, double height, BodyType bodyType,
		Level* levelPtr, void* userPointer = nullptr);
	virtual ~Enemy();

	Enemy(const Enemy&) = delete;
	Enemy& operator=(const Enemy&) = delete;

	void Tick(double deltaTime);
	virtual void Paint() = 0;

	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;

	Type GetType() const;

	static std::string TYPEToString(Type type);
	static Type StringToTYPE(const std::string& string);

protected:
	static const int MINIMUM_PLAYER_DISTANCE; // how close the player needs to get for us to activate

	Type m_Type;
	DOUBLE2 m_SpawingPosition;

	bool m_IsActive; // This only gets set to true when the player is nearby

	int m_DirFacing;
	int m_DirFacingLastFrame;

	bool m_IsOnGround;

};
