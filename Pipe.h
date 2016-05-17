#pragma once

class Level;
class Player;

class Pipe
{
public:
	enum class Orientation
	{
		LEFT, RIGHT, UP, DOWN, NONE
	};

	Pipe(DOUBLE2 topLeft, DOUBLE2 bottomRight, Level* levelPtr, Orientation orientation, 
		int index, int warpLevelIndex = -1, int warpPipeIndex = -1);
	virtual ~Pipe();

	Pipe(const Pipe&) = delete;
	Pipe& operator=(const Pipe&) = delete;

	void AddContactListener(ContactListener* listener);
	bool Raycast(DOUBLE2 point1, DOUBLE2 point2, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef);

	Orientation GetOrientation();

	static Orientation StringToOrientation(std::string orientationStr);
	static std::string OrientationToString(Orientation orientation);

	DOUBLE2 GetWarpToPosition();

	bool IsPlayerInPositionToEnter(Player* playerPtr);

	int GetIndex();
	int GetWarpPipeIndex();
	int GetWarpLevelIndex();

private:
	PhysicsActor* m_ActPtr = nullptr;
	RECT2 m_Bounds;

	Orientation m_Orientation;

	int m_Index; // How many pipes were added before this one (used for pipe warping)

	bool m_IsWarpPipe = false; // Is true when mario can warp through this pipe
	int m_WarpLevelIndex = -1; // What level index this pipe warps the player to
	int m_WarpPipeIndex = -1; // Which pipe the player will come out in warped to level
};
