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

	Pipe(DOUBLE2 topLeft, DOUBLE2 bottomRight, Level* levelPtr, Orientation orientation, bool canAccess = false);
	virtual ~Pipe();

	Pipe(const Pipe&) = delete;
	Pipe& operator=(const Pipe&) = delete;

	void AddContactListener(ContactListener* listener);
	bool Raycast(DOUBLE2 point1, DOUBLE2 point2, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef);

	Orientation GetOrientation();

	static Orientation StringToOrientation(std::string orientationStr);
	static std::string OrientationToString(Orientation orientation);

	bool IsPlayerInPositionToEnter(Player* playerPtr);

private:
	PhysicsActor* m_ActPtr = nullptr;
	RECT2 m_Bounds;

	Orientation m_Orientation;

	// If true mario can enter this pipe
	bool m_CanAccess;
};
