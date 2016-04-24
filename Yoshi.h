#pragma once

#include "Entity.h"

class Yoshi : public Entity
{
public:
	enum class ANIMATION_STATE
	{
		EGG, BABY, WAITING, WALKING, WILD, JUMPING, FALLING, EATING, DUCKING
	};

	Yoshi(DOUBLE2 position, Level* levelPtr);
	virtual ~Yoshi();

	void Tick(double deltaTime);
	void Paint();

	int GetWidth();
	int GetHeight();

	bool IsHatching();

private:
	void PaintAnimationFrame(double left, double top);
	void ChangeAnimationState(ANIMATION_STATE newAnimationState);

	static const int WIDTH = 16;
	static const int HEIGHT = 14;

	ANIMATION_STATE m_AnimationState;

	bool m_IsRunning;
	bool m_IsCarryingMario;
	bool m_IsTurningAround;
	
	CountdownTimer m_HatchingTimer;
	SpriteSheet* m_SpriteSheetPtr = nullptr;

};

