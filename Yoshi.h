#pragma once

#include "Entity.h"

class Player;

class Yoshi : public Entity
{
public:
	enum class ANIMATION_STATE
	{
		EGG, BABY, WAITING, WALKING, WILD, JUMPING, FALLING, EATING, DUCKING
	};

	Yoshi(DOUBLE2 position, Level* levelPtr);
	virtual ~Yoshi();

	Yoshi(const Yoshi&) = delete;
	Yoshi&operator=(const Yoshi&) = delete;

	void Tick(double deltaTime);
	void Paint();

	int GetWidth();
	int GetHeight();

	bool IsHatching();
	void SetCarryingMario(bool carryingMario, Player* marioPtr = nullptr);

private:
	void PaintAnimationFrame(double left, double top);
	void ChangeAnimationState(ANIMATION_STATE newAnimationState);

	static const int WIDTH = 16;
	static const int HEIGHT = 14;

	ANIMATION_STATE m_AnimationState;

	// NOTE: Yoshi enters this state when mario takes damage while riding yoshi and is forced to dismount
	bool m_IsRunning;
	bool m_IsCarryingMario;
	bool m_IsTurningAround;
	
	CountdownTimer m_HatchingTimer;
	SpriteSheet* m_SpriteSheetPtr = nullptr;

	Player* m_PlayerPtr = nullptr;

};
