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

class PhysicsPrismaticJoint : public PhysicsJoint
{
public:
	//! A prismatic joint (aka slider joint) allows for relative translation of two bodies along a specified axis. A prismatic joint prevents relative rotation.Therefore, a prismatic joint has a single degree of freedom.
	//! To specify a prismatic you need to provide two physics actors. The initialization function assumes that the physics actors are already in the correct position.
	//! Always delete joint before deleting the attached actors
	//! Deleting an actor attached to the joint will allways delete the internal Box2D joint.
	//! @param actAPtr: The first actor that is attached to this joint.
	//! @param anchorA: The local anchor point relative to actA's origin.
	//! @param actBPtr: The second actor that is attached to this joint.
	//! @param anchorB: The local anchor point relative to actB's origin.
	//! @param jointAxis: The axis along which the physics actors will move according to each other. This axis is fixed in the two bodies and follows their motion.
	//! @param collide: if true the actors attached to this joint will collide
	PhysicsPrismaticJoint(PhysicsActor *actAPtr, DOUBLE2 anchorA, PhysicsActor *actBPtr, DOUBLE2 anchorB, DOUBLE2 jointAxis, bool collide = false);
	virtual ~PhysicsPrismaticJoint();

	// C++11 make the class non-copyable
	PhysicsPrismaticJoint(const PhysicsPrismaticJoint&) = delete;
	PhysicsPrismaticJoint& operator=(const PhysicsPrismaticJoint&) = delete;

	//! A joint limit forces the joint angle to remain between a lower and upper bound. The limit will apply as	much torque as needed to make this happen. The limit range should include zero, otherwise the joint	will lurch when the simulation begins.
	//! @param enableLimits: enable or disable the translation limits
	//! @param lowerTranslation: if enabled, his specifies the lower translation
	//! @param upperTranslation: if enabled, his specifies the upper translation
	void EnableJointLimits(bool enableLimits, double lowerTranslation, double upperTranslation);

	//! A joint motor allows you to specify the joint speed (the time derivative of the angle). The speed can be negative or positive.A motor can have infinite force, but this is usually not desirable
	//! @param enableMotor: enable or disable the motor
	//! @param targetSpeed: if enabled, his specifies the target speed in pixels per second
	//! @param maxMotorForce: if enabled, his specifies the maximum force
	void EnableMotor(bool enableMotor, double targetSpeed = 0, double maxMotorForce = 0);

	//! returns the current translation of the motor
	double GetJointTranslation() const;

	//! returns true if the limits are enabled
	bool IsLimitEnabled()const; 

	//! returns the lower translation limit
	double GetLowerLimit()const; 

	//! returns the upper translation limit
	double GetUpperLimit()const; 

	//! returns true if the motor is enabled
	bool IsMotorEnabled()const; 

	//! returns the current motor speed
	double GetMotorSpeed()const;

	//! returns the current motor force
	double GetMotorForce(double deltaTime)const;

	b2PrismaticJoint* GetBox2dObj() const { return m_PrismaticJointPtr; }

private:				 
	b2PrismaticJoint *m_PrismaticJointPtr = nullptr;

};

