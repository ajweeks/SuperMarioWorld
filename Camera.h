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

private:
	static const int MAX_RIGHT_OFFSET = 200;
	static const int MAX_LEFT_OFFSET = 60;

	void TrackAvatar(DOUBLE2& posRef, Player* playerPtr);
	void Clamp(DOUBLE2& posRef, Level* levelPtr);

	int m_Width;
	int m_Height;

	// How far from the left side of the screen the player is
	DOUBLE2 m_PrevTranslation;
	DOUBLE2 m_TargetOffset;
	double m_XOffset;
};
