//-----------------------------------------------------------------
// Game Engine
// C++ Source - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------
#include "stdafx.h"
#include "PhysicsJoint.h"

void PhysicsJoint::SetUserData(int data)
{
	m_UserData = data;
}

int PhysicsJoint::GetUserData()
{
	return m_UserData;
}