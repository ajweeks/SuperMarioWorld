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

class PhysicsRevoluteJoint : public PhysicsJoint
{
public:
	//! A revolute joint forces two physics actors to share a common anchor point, often called a hinge point. The revolute joint has a single degree of freedom : the relative rotation of the two physics	actors. This is called the joint angle.
	//! To specify a revolute you need to provide two physics actors and a single anchor point in world space. The initialization function assumes that the physics actors are already in the correct position.
	//! Always delete joint before deleting the attached actors
	//! Deleting an actor attached to the joint will allways delete the internal Box2D joint.
	//! @param actAPtr: The first actor that is attached to this joint.
	//! @param anchorA: The local anchor point relative to actA's origin.
	//! @param actBPtr: The second actor that is attached to this joint.
	//! @param anchorB: The local anchor point relative to actB's origin.
	//! @param collide: if true the actors attached to this joint will collide
	//! @param referenceAngle: an angle between bodies considered to be zero for the joint angle. See http://www.iforce2d.net/b2dtut/joints-revolute
	PhysicsRevoluteJoint(PhysicsActor *actAPtr, DOUBLE2 anchorA, PhysicsActor *actBPtr, DOUBLE2 anchorB, bool collide = false, double referenceAngle = 0);
	virtual ~PhysicsRevoluteJoint();
	// C++11 make the class non-copyable
	PhysicsRevoluteJoint(const PhysicsRevoluteJoint&) = delete;
	PhysicsRevoluteJoint& operator=(const PhysicsRevoluteJoint&) = delete;

	//! A joint limit forces the joint angle to remain between a lower and upper bound. The limit will apply as	much torque as needed to make this happen. The limit range should include zero, otherwise the joint	will lurch when the simulation begins.
	//! @param enableLimits: enable or disable the angle limits
	//! @param lowerAngle: if enabled, his specifies the lower angle
	//! @param upperAngle: if enabled, his specifies the upper angle
	void EnableJointLimits(bool enableLimits, double lowerAngle, double upperAngle);

	//! A joint motor allows you to specify the joint speed (the time derivative of the angle). The speed can be negative or positive.A motor can have infinite force, but this is usually not desirable
	//! @param enableMotor: enable or disable the motor
	//! @param targetSpeed: if enabled, his specifies the target speed in radians per second
	//! @param maxMotorTorque: if enabled, his specifies the maximum torque
	void EnableMotor(bool enableMotor, double targetSpeed = 0, double maxMotorTorque = 0);

	//! returns true if the limits are enabled
	bool IsLimitEnabled()const;

	//! returns the lower limit angle in radians
	double GetLowerLimit()const;

	//! returns the upper limit angle in radians
	double GetUpperLimit()const;

	//! returns true if the motor is enabled
	bool IsMotorEnabled()const;

	//! returns the motor speed in radians per second
	double GetMotorSpeed()const;

	//! returns the angle of the joint in radians
	double GetJointAngle() const;

	//! returns the reference angle
	double GetReferenceAngle() const;

	//! returns the torque
	//! @param deltaTime is the time as given by the GameTick method
	double GetMotorTorque(double deltaTime)const;

	b2RevoluteJoint* GetBox2dObj() const { return m_RevoluteJointPtr; }

private:
	b2RevoluteJoint *m_RevoluteJointPtr = nullptr;

};

