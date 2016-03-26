//-----------------------------------------------------------------
// Game Engine
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

#pragma once
#include "PhysicsJoint.h"

class PhysicsActor;

class PhysicsDistanceJoint : public PhysicsJoint
{
public:
	//! PhysicsDistanceJoint: the distance between two points on two bodies must be constant
	//! Joints between static and/or kinematic bodies are allowed, but have no effect and use some processing time.
	//! Always delete joint before deleting the attached actors
	//! Deleting an actor attached to the joint will allways delete the internal Box2D joint.
	//! @param actAPtr: The first actor that is attached to this joint.
	//! @param anchorA: The local anchor point relative to actA's origin.
	//! @param actBPtr: The second actor that is attached to this joint.
	//! @param anchorB: The local anchor point relative to actB's origin.
	//! @param length: The natural length between the anchor points.
	//! @param frequencyHz: The mass-spring-damper frequency in Hertz. A value of 0
	//! @param dampingRatio: disables softness. The damping ratio. 0 = no damping, 1 = critical damping.
	PhysicsDistanceJoint(PhysicsActor *actAPtr, DOUBLE2 anchorA, PhysicsActor *actBPtr, DOUBLE2 anchorB, double length = 1, double frequencyHz = 0, double dampingRatio = 0);
	virtual ~PhysicsDistanceJoint();
	// C++11 make the class non-copyable
	PhysicsDistanceJoint(const PhysicsDistanceJoint&) = delete;
	PhysicsDistanceJoint& operator=(const PhysicsDistanceJoint&) = delete;

	//! Get the distance between the two anchor locations
	double GetLength() const;

	//! Set the distance between the anchor locations
	//! @param length: The natural length between the anchor points.
	void SetLength(double length);

	double GetFrequency() const;
	void SetFrequency(double frequency);

	double GetDampingRatio() const;
	void SetDampingRatio(double dampingRatio);

	//! Get the reaction force applied to actorA at the anchor point in Newton. 
	//! This reaction force can be used to break joints or trigger other game events
	//! @param deltaTime is the time as given by the GameTick method
	DOUBLE2 GetReactionForce(double deltaTime) const;

	//! Get the reaction torque applied to actorA at the anchor point in Newton. 
	//! This reaction force can be used to break joints or trigger other game events
	//! Unit is N*m. This is always zero for a distance joint.
	//! @param deltaTime is the time as given by the GameTick method
	double GetReactionTorque(double deltaTime) const;

private:				 
	b2DistanceJoint *m_DistanceJointPtr = nullptr;
};

