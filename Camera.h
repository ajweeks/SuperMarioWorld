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

	MATRIX3X2 GetViewMatrix(Player* playerPtr, Level* levelPtr);

	void Reset();

	void DEBUGPaint();

private:
	// NOTE: How close to either side of the screen the player can get
	static const int MAX_OFFSET = 140;
	// NOTE: How much the player can walk backwards before triggering a camera transition
	static const int MAX_BACKTRACK_DISTANCE = 26;

	void Clamp(DOUBLE2& posRef, Level* levelPtr);

	// Generally set to the same size as Game::WIDTH,Game::HEIGHT, unless debugging
	int m_Width, m_Height;

	DOUBLE2 m_PrevTranslation; // How far from the left side of the screen the player is
	DOUBLE2 m_TargetOffset;
	double m_XOffset;
};
