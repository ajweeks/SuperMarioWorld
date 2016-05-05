#pragma once

class Player;
class Level;

class Camera
{
public:
	Camera(int width, int height, Level* levelPtr);
	virtual ~Camera();

	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;

	DOUBLE2 GetOffset(DOUBLE2 playerPos, int directionFacing, Level* levelPtr);
	MATRIX3X2 GetViewMatrix(DOUBLE2 playerPos, int directionFacing, Level* levelPtr);

	void Reset();

	void DEBUGPaint();

private:
	double CalculateXTranslation(DOUBLE2 playerPos, int directionFacing, Level* levelPtr);
	double CalculateYTranslation(DOUBLE2 playerPos, Level* levelPtr);

	void Clamp(DOUBLE2& posRef, Level* levelPtr);

	// NOTE: How close to either side of the screen the player can get
	static const int MAX_X_OFFSET = 140;
	// NOTE: How far the player can walk backwards before triggering a camera transition
	static const int HORIZONTAL_CUSHION_SIZE = 26;

	static const int TOP_BOUNDARY = 106;
	static const int BOTTOM_BOUNDARY = 150;

	// This is set to true when the player climbs on a beanstalk
	bool m_UsingConstantYO = false;

	// Generally set to the same size as Game::WIDTH,Game::HEIGHT, unless debugging
	int m_Width, m_Height;

	// How far from the left side of the screen the player is
	DOUBLE2 m_PrevTranslation; 
	DOUBLE2 m_TargetOffset;

	double m_XOffset;
};
