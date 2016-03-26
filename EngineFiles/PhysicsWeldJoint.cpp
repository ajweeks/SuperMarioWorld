//-----------------------------------------------------------------
// Game Engine
// C++ Source - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------
#include "stdafx.h"
#include "../stdafx.h" // for intellisense

#include "PhysicsWeldJoint.h"
#include "PhysicsActor.h"

PhysicsWeldJoint::PhysicsWeldJoint(PhysicsActor *actAPtr, DOUBLE2 anchorA, PhysicsActor *actBPtr, DOUBLE2 anchorB, double frequencyHz, double dampingRatio)
{
	b2WeldJointDef weldJointDef;
	weldJointDef.bodyA = actAPtr->m_BodyPtr;
	weldJointDef.bodyB = actBPtr->m_BodyPtr;
	weldJointDef.localAnchorA = b2Vec2((float)anchorA.x / (float)PhysicsActor::SCALE, (float)anchorA.y / (float)PhysicsActor::SCALE);
	weldJointDef.localAnchorB = b2Vec2((float)anchorB.x / (float)PhysicsActor::SCALE, (float)anchorB.y / (float)PhysicsActor::SCALE);
	weldJointDef.frequencyHz = (float)frequencyHz;
	weldJointDef.dampingRatio = (float)dampingRatio;

	m_WeldJointPtr = dynamic_cast <b2WeldJoint*>((GameEngine::GetSingleton())->GetBox2DWorld()->CreateJoint(&weldJointDef));
	m_WeldJointPtr->SetUserData(this);
}

PhysicsWeldJoint::~PhysicsWeldJoint()
{
	if (!m_Deleted)
		(GameEngine::GetSingleton())->GetBox2DWorld()->DestroyJoint(m_WeldJointPtr);
}

double PhysicsWeldJoint::GetFrequency() const
{
	return m_WeldJointPtr->GetFrequency();
}

void PhysicsWeldJoint::SetFrequency(double frequency)
{
	m_WeldJointPtr->SetFrequency((float)frequency);
}


double PhysicsWeldJoint::GetDampingRatio() const
{
	return m_WeldJointPtr->GetDampingRatio();
}

void PhysicsWeldJoint::SetDampingRatio(double dampingRatio)
{
	m_WeldJointPtr->SetDampingRatio((float)dampingRatio);
}

DOUBLE2 PhysicsWeldJoint::GetReactionForce(double deltaTime) const
{
	b2Vec2 vec2 = m_WeldJointPtr->GetReactionForce(1/(float)deltaTime);
	return DOUBLE2(vec2.x, vec2.y);
}

double PhysicsWeldJoint::GetReactionTorque(double deltaTime) const
{
	return m_WeldJointPtr->GetReactionTorque(1 / (float)deltaTime);
}
