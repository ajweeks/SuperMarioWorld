#pragma once

#include "SMWMultiTimer.h"

class Player;
class Camera;

class EndScreen
{
public:
	virtual ~EndScreen();

	static void Initalize(Player* playerPtr, Camera* cameraPtr, int extraTime, int score, int bonusScore);
	static bool Tick();
	static void Paint();

private:
	EndScreen();
	static void PaintText();
	static void PaintEnclosingCircle(DOUBLE2 circleCenter, double innerCircleRadius);

	static const int FIRST_FADE_END;
	static const int FIRST_FADE_END_VALUE;
	static const int SECOND_FADE_BEGIN;
	static const int SECOND_FADE_BEGIN_VALUE;
	static const int SECOND_FADE_END;
	static const int SECOND_FADE_END_VALUE;
	static const int ENCLOSING_CIRCLE_END;
	static const int ENCLOSING_CIRCLE_END_VALUE;

	static const int DRUMROLL_START_POSITION;
	
	static SMWMultiTimer m_EndScreenTransitionMultiTimer;

	static Player* m_PlayerPtr;
	static Camera* m_CameraPtr;
	static int m_ExtraTime;
	static int m_ScoreShowing;
	static int m_BonusScoreShowing;
};

