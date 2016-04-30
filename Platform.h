#pragma once

class Platform
{
public:
	// NOTE: All platforms have the same height, this prevents too thin or thick 
	// of platforms from accidentally being made
	Platform(double left, double top, double right);
	virtual ~Platform();

	Platform(const Platform&) = delete;
	Platform&operator=(const Platform&) = delete;

	void AddContactListener(ContactListener* listener);
	double GetWidth();
	double GetHeight();
	bool Raycast(DOUBLE2 point1, DOUBLE2 point2, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef);

	static const int HEIGHT = 6;

private:
	PhysicsActor* m_ActPtr = nullptr;
	double m_Width;

	// TODO: Add angled platforms
	// TODO: Add moving platforms
};
