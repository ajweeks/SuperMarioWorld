//-----------------------------------------------------------------
// Game Engine
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

#pragma once
class PhysicsJoint
{
public:
	PhysicsJoint(){};
	virtual ~PhysicsJoint(){};

	//! Sets the userdata of this Joint
	//! @param data is the int containing a value that the programmer(user) can define
	//! Use this to identify the Joint if only the pointer of the actor is known
	//! Best practice is to use a enumeration type (not class enum type)
	void SetUserData(int data);

	//! Returns the userdata of this Joint
	int GetUserData();

protected:
	bool m_Deleted = false;

private:
	friend class GameEngine;
	int m_UserData = 0;
};

