//-----------------------------------------------------------------
// Game Engine
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------
#pragma once

//-----------------------------------------------------
// ContactListener Class									
//-----------------------------------------------------
class ContactListener
{
public:
	ContactListener( );
	virtual ~ContactListener( );

	// C++11 make the class non-copyable
	ContactListener( const ContactListener& ) = delete;
	ContactListener& operator=( const ContactListener& ) = delete;

	//! BeginContact is called by the GameEngine when contact between a trigger ContactListener and another ContactListener is made.
	//! @param actThisPtr is the pointer of the actor that is a contactListener.
	//! @param actOtherPtr is the pointer of the actor that made contact with actThisPtr. It can be a ContactListener too.
	//! NEVER modify physicsactors in de body of this method
	virtual void BeginContact(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr) {}; 

	//! BeginContact is called by the GameEngine when contact between a trigger ContactListener and another ContactListener has ended.
	//! @param actThisPtr is the pointer of the actor that is a contactListener.
	//! @param actOtherPtr is the pointer of the actor that made contact with actThisPtr. It can be a ContactListener too.
	//! NEVER modify physicsactors in de body of this method
	virtual void EndContact(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr) {};

	//! ContactImpulse in called by the GameEngine when the Listener had contact.
	//! @param impulse is calculated by Box2D to push the actors out of each other. The bigger the impulse, the harder they have hit each other
	//! NEVER modify physicsactors in de body of this method
	virtual void ContactImpulse(PhysicsActor *actThisPtr, double impulse) {};

	//! PreSolve is called by the GameEngine right before the collision is solved.
	//! @param enableContactRef: when this referenced parameter is set to false, the contact is ignored, 
	//! making actors pass through each other.
	//! NEVER modify physicsactors in de body of this method
	virtual void PreSolve(PhysicsActor *actThisPtr, PhysicsActor *actOtherPtr, bool & enableContactRef){};
};

 
