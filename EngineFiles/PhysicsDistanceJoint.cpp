//-----------------------------------------------------------------
// Game Engine
// C++ Source - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------
#include "stdafx.h"
#include "../stdafx.h" // for intellisense

#include "PhysicsDistanceJoint.h"
#include "PhysicsActor.h"

PhysicsDistanceJoint::PhysicsDistanceJoint(PhysicsActor *actAPtr, DOUBLE2 anchorA, PhysicsActor *actBPtr, DOUBLE2 anchorB, double length, double frequencyHz, double dampingRatio)
{
	b2DistanceJointDef DistanceJointDef;
	DistanceJointDef.bodyA = actAPtr->m_BodyPtr;
	DistanceJointDef.bodyB = actBPtr->m_BodyPtr;
	DistanceJointDef.localAnchorA = b2Vec2((float)anchorA.x / (float)PhysicsActor::SCALE, (float)anchorA.y / (float)PhysicsActor::SCALE);
	DistanceJointDef.localAnchorB = b2Vec2((float)anchorB.x / (float)PhysicsActor::SCALE, (float)anchorB.y / (float)PhysicsActor::SCALE);
	DistanceJointDef.length = (float)length / (float)PhysicsActor::SCALE;
	DistanceJointDef.frequencyHz = (float)frequencyHz;
	DistanceJointDef.dampingRatio = (float)dampingRatio;

	m_DistanceJointPtr = dynamic_cast <b2DistanceJoint*>((GameEngine::GetSingleton())->GetBox2DWorld()->CreateJoint(&DistanceJointDef));
	m_DistanceJointPtr->SetUserData(this);
}

PhysicsDistanceJoint::~PhysicsDistanceJoint()
{
	if (!m_Deleted)
		(GameEngine::GetSingleton())->GetBox2DWorld()->DestroyJoint(m_DistanceJointPtr);
}

double PhysicsDistanceJoint::GetLength() const
{
	return m_DistanceJointPtr->GetLength() * (float)PhysicsActor::SCALE;
}

void PhysicsDistanceJoint::SetLength(double length)
{
	if (length < 0.1)
	{
		GameEngine::GetSingleton()->MessageBox(String("Length can not be a very small number "));
	}
	m_DistanceJointPtr->SetLength((float)length / (float)PhysicsActor::SCALE);
}


double PhysicsDistanceJoint::GetFrequency() const
{
	return m_DistanceJointPtr->GetFrequency();
}

void PhysicsDistanceJoint::SetFrequency(double frequency)
{
	m_DistanceJointPtr->SetFrequency((float)frequency);
}


double PhysicsDistanceJoint::GetDampingRatio() const
{
	return m_DistanceJointPtr->GetDampingRatio();
}

void PhysicsDistanceJoint::SetDampingRatio(double dampingRatio)
{
	m_DistanceJointPtr->SetDampingRatio((float)dampingRatio);
}

DOUBLE2 PhysicsDistanceJoint::GetReactionForce(double deltaTime) const
{
	b2Vec2 vec2 = m_DistanceJointPtr->GetReactionForce(1/(float)deltaTime);
	return DOUBLE2(vec2.x, vec2.y);
}

double PhysicsDistanceJoint::GetReactionTorque(double deltaTime) const
{
	return m_DistanceJointPtr->GetReactionTorque(1 / (float)deltaTime);
}
