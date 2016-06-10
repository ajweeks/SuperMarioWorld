#pragma once

#include "SMWTimer.h"

class Player;
class Level;

class Camera
{
public:
	Camera(int width, int height, Level* levelPtr);
	virtual ~Camera();

	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;

	DOUBLE2 GetOffset(Level* levelPtr, double deltaTime);
	void CalculateViewMatrix(Level* levelPtr, double deltaTime);
	MATRIX3X2 GetViewMatrix();

	void Reset();
	void DEBUGPaint();

private:
	double CalculateXOffset(Level* levelPtr, double deltaTime);
	double CalculateYOffset(Level* levelPtr, double deltaTime);

	void Clamp(DOUBLE2& posRef, Level* levelPtr);

	// How far from the edge the player is from the edge of the screen normally
	static const int DISTANCE_FROM_EDGE;
	
	// How far the player can walk backwards before triggering a camera transition
	static const int HORIZONTAL_CUSHION_SIZE;
	
	// How quickly a camera transition occurs (percentage)
	//static const double HORIZONTAL_PAN_SPEED;

	static const int TOP_BOUNDARY;
	static const int TOP_BOUNDARY_LOW;
	static const int BOTTOM_BOUNDARY;
	static const int FRAMES_OF_TRANSITION;
	
	static const int DEFAULT_Y_OFFSET;

	const int WIDTH, HEIGHT;

	DOUBLE2 m_PrevOffset;
	int m_OffsetDirection;

	SMWTimer m_TransitionTimer;

	// Stores the distance between the player and the camera
	double m_XOffset;

	MATRIX3X2 m_MatTranslation;
};
