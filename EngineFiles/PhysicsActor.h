//-----------------------------------------------------------------
// Game Engine
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

// The PhysicsActor class encapsulates the b2Body class and adds methods to it to handle fixtures

#pragma once

//-----------------------------------------------------------------
// PhysicsActor Class
//-----------------------------------------------------------------
class PhysicsActor
{
public:
	//! constructor
	//! @param pos the position of the actor
	//! @param angle the angle of rotation of this actor
	//! @param bodyType BodyType is an Enumeration with three possible values. Eg. use BodyType::DYNAMIC
	PhysicsActor(DOUBLE2 pos, double angle, BodyType bodyType);

	//! constructor
	//! @param posX the x coordinate of the position of the actor
	//! @param posY the y coordinate of the position of the actor
	//! @param angle the angle of rotation of this actor
	//! @param bodyType BodyType is an Enumeration with three possible values. Eg. use BodyType::DYNAMIC
	PhysicsActor(double posX, double posY, double angle, BodyType bodyType);

	//! Destructor
	virtual ~PhysicsActor();

	// C++11 make the class non-copyable
	PhysicsActor(const PhysicsActor&) = delete;
	PhysicsActor& operator=(const PhysicsActor&) = delete;

	//! Returns the encapsulated body. Use this body to call box2D methods that are not wrapped by this class.
	b2Body* GetBody();

	//! Adds a box fixture to this actor, the box is centered around the position of the actor
	//! @param width represents the width of the physics box 
	//! @param height represents the height of the physics box 
	//! @param restitution is the a value between 0 (full damping) and 1 (no energy loss). The default value is 0.8
	//! @param friction represents the static and dynamic friction.  The friction parameter is
	//! usually set between 0 and 1, but can be any non - negative value. A friction value of 0 turns off friction
	//!	and a value of 1 makes the friction strong. When the friction force is computed between two fixtures,
	//!	Box2D must combine the friction parameters of the two parent fixtures.
	//! @param density represents the density of this fixture.  The default density is 1 
	//! @return a pointer to the created b2Fixture if the operation was successful
	b2Fixture* AddBoxFixture(double width, double height, double restitution = 0.8, double friction = 0.7, double density = 1);

	//! Adds a circle fixture to the actor, the circle is centered around the position of the actor
	//! @param radius is the radius of the circle
	//! @param offset is position, relative to the body position
	//! @param restitution is the a value between 0 (full damping) and 1 (no energy loss). The default value is 0.8
	//! @param friction represents the static and dynamic friction.  The friction parameter is
	//! usually set between 0 and 1, but can be any non - negative value. A friction value of 0 turns off friction
	//!	and a value of 1 makes the friction strong. When the friction force is computed between two fixtures,
	//!	Box2D must combine the friction parameters of the two parent fixtures.
	//! @param density represents the density of this fixture.  The default density is 1 
	//! @return a pointer to the created b2Fixture if the operation was successful
	b2Fixture* AddCircleFixture(double radius, DOUBLE2 offset = DOUBLE2(0, 0), double restitution = 0.8, double friction = 0.7, double density = 1);

	//! Adds a polygon fixture to the actor, the polygon coordinates are relative to the position of the actor. Polygons must be convex. 
	//! @param vertexArrRef is a std::vector containing DOUBLE2 values that are representing the coordinates of the points of the polygon. There is a limit of 8 vertices per polygon!
	//! @param restitution is the a value between 0 (full damping) and 1 (no energy loss)
	//! @param friction represents the static and dynamic friction.  The friction parameter is
	//! usually set between 0 and 1, but can be any non - negative value. A friction value of 0 turns off friction
	//!	and a value of 1 makes the friction strong. When the friction force is computed between two fixture,
	//!	Box2D must combine the friction parameters of the two parent fixtures.
	//! @param density represents the density of this fixture.  The default density is 1 
	//! @return a pointer to the created b2Fixture if the operation was successful
	b2Fixture* AddPolygonFixture(const std::vector<DOUBLE2> & vertexArrRef, double restitution = 0.8, double friction = 0.7, double density = 1);

	//! Adds a chain fixture to the actor. A chain is a line list that provides two-sided collision.
	//! Torque can not be applied to this fixture!!
	//! @param vertexArrRef is a std::vector containing DOUBLE2 values that are representing the coordinates of the points of the line list
	//! @param restitution is the a value between 0 (full damping) and 1 (no energy loss)
	//! @param friction represents the static and dynamic friction.  The friction parameter is
	//! usually set between 0 and 1, but can be any non - negative value. A friction value of 0 turns off friction
	//!	and a value of 1 makes the friction strong. When the friction force is computed between two fixture,
	//!	Box2D must combine the friction parameters of the two parent fixtures.
	//! @param density represents the density of this fixture.  The default density is 1 
	//! @return a pointer to the created b2Fixture if the operation was successful
	b2Fixture* AddChainFixture(const std::vector<DOUBLE2> & vertexArrRef, bool closed, double restitution = 0.8, double friction = 0.7, double density = 1);

	//! Parses a svg file and extracts lines only. Supports only SVG1.0 and 1.1 text files. Use Inkscape to create the svg file.
	//! Converts the lines into chains and adds them to the actor. 
	//! @param svgFilePathRef is a String representing the name of the svg file to parse.
	//! @param restitution is the a value between 0 (full damping) and 1 (no energy loss)
	//! @param friction represents the static and dynamic friction.  The friction parameter is usually set between 0 and 1, but can be any non - negative value. A friction value of 0 turns off friction and a value of 1 makes the friction strong. When two fixtures collide with each other, the resulting friction tends toward the lower of their friction values.
	//! @param density represents the density of this fixture.  The default density is 1 
	//! @return a vector containing a list of the created b2Fixtures 
	std::vector<b2Fixture *> AddSVGFixture(const String & svgFilePathRef, double restitution = 0.8, double friction = 0.7, double density = 1);

	//! Parses a svg file and extracts lines only. Supports only SVG1.0 and 1.1 text files. Use Inkscape to create the svg file.
	//! Converts the lines into chains and adds them to the actor. 
	//! @param resourceID is a resourceID representing the svg file to parse.
	//! @param restitution is the a value between 0 (full damping) and 1 (no energy loss)
	//! @param friction represents the static and dynamic friction.  The friction parameter is usually set between 0 and 1, but can be any non - negative value. A friction value of 0 turns off friction and a value of 1 makes the friction strong. When two fixtures collide with each other, the resulting friction tends toward the lower of their friction values.
	//! @param density represents the density of this fixture.  The default density is 1 
	//! @return a vector containing a list of the created b2Fixtures 
	std::vector<b2Fixture *> AddSVGFixture(int resourceID, double restitution = 0.8, double friction = 0.7, double density = 1);

	//! Parses a svg file and extracts lines only. Supports only SVG1.0 and 1.1 text files. Use Inkscape to create the svg file.
	//! Converts the lines into chains and adds them to the actor. 
	//! @param pBlob is a memory chunk pointer representing the svg file to parse.
	//! @param blobsize is the number of bytes contained in the blob.
	//! @param restitution is the a value between 0 (full damping) and 1 (no energy loss)
	//! @param friction represents the static and dynamic friction.  The friction parameter is usually set between 0 and 1, but can be any non - negative value. A friction value of 0 turns off friction and a value of 1 makes the friction strong. When two fixtures collide with each other, the resulting friction tends toward the lower of their friction values.
	//! @param density represents the density of this fixture.  The default density is 1 
	//! @return a vector containing a list of the created b2Fixtures 
	std::vector<b2Fixture *> AddSVGFixture(BYTE* pBlob, int blobsize, double restitution = 0.8, double friction = 0.7, double density = 1);

	//! removes a fixture from the list 
	//! @param fixturePtr is the b2Fixture to be removed
	void DestroyFixture(b2Fixture *fixturePtr);

	//! Sets the userdata of this actor
	//! @param data is the int containing a value that the programmer(user) can define
	//! Use this to identify the actor if only the pointer of the actor is known
	//! Best practice is to use a enumeration type (not class enum type)
	void SetUserData(int data);

	//! Returns the userdata of this actor
	int GetUserData()const;

	//! Sets the user pointer
	void SetUserPointer(void *ptr);

	//! Gets the user pointer
	void * GetUserPointer();

	//! Gets the 
	//! Sets the positon of this actor
	//! param posX is the new horzontal position for this actor
	//! param posY is the new vertical position for this actor
	void SetPosition(double posX, double posY);

	//! Sets the positon of this actor
	//! param position is the new position for this actor
	void SetPosition(const DOUBLE2& positionRef);

	//! Returns the position of this actor
	DOUBLE2 GetPosition()const;

	//! Sets the actors angle
	void SetAngle(double angle);

	//! Returns the angle of this actor
	double GetAngle() const;

	//! Sets velocity of the actor
	//! @param velocity is the new velocity
	void SetLinearVelocity(DOUBLE2 velocity);

	//! Gets velocity of the actor
	//! @return the velocity vector
	DOUBLE2 GetLinearVelocity() const;

	//! Sets angular velocity of the actor
	//! @param velocity is the new angular velocity
	void SetAngularVelocity(double velocity);

	//! Gets the angular velocity of the actor
	//! @return the angular velocity
	double GetAngularVelocity() const;

	//! Returns the total mass of the actor
	double GetMass() const;

	//! Enables or disables rotation of this actor
	//! @param fixedRotation when set to true, it disables rotation of this actor
	void SetFixedRotation(bool fixedRotation);

	//! Returns the state of fixed rotation.
	//! true: rotation is fixed
	//! false: rotation enabled
	bool GetFixedRotation() const;

	//! prevent dynamic bodies from tunneling through dynamic bodies. Use it wisely for performance reasons.
	//! @param bullet if true, this treats the actor as a fast moving entity
	void SetBullet(bool bullet);

	bool GetBullet();

	//! You may wish a body to be created but not participate in collision or dynamics. 
	//! @param active if false this will prevent the actor to participate in collision or dynamics
	void SetActive(bool active);

	//! You may wish to know if a body participates in collision or dynamics. 
	//! @return true when active, false if not.
	bool IsActive();

	//! each Actor has a 'gravity scale' to strengthen or weaken the effect of the world's gravity on it.
	//! @param scale e.g. when set to 0 the object will not react to gravity, 1 is normal gravity
	void SetGravityScale(double scale);

	//! each Actor has a 'gravity scale' to strengthen or weaken the effect of the world's gravity on it.
	//! @return the scale: e.g. when set to 0 the object will not react to gravity, 1 is normal gravity
	double GetGravityScale();

	//! Changes the bodytype (static or dynamic or kinematic)
	//! @param: the bodytype
	void SetBodyType(BodyType bodyType);

	//! Returns the bodyType (static or dynamic or kinematic)
	BodyType GetBodyType();

	//! Actors that are not moving can be set to a non active state, enhancing the performance
	//! use this method to force an actor to wake up
	void SetAwake(bool flag);

	//! Actors that are not moving can be set to a non actove state, enhancing the performance
	//! use this method to find out the awake state
	bool IsAwake() const;
	
	//! setting to true will cause the actor to be like a ghost, no collisions with other actors.
	//! Trigger events will be generated when another actor overlaps.
	//! @param isSensor set to true will cause the actor to be like a sensor
	// see also http://www.iforce2d.net/b2dtut/sensors
	void SetSensor(bool isSensor);
	
	//! When a actor is a sensor, it will cause the actor to be like a ghost, no collisions with other actors.
	//! Trigger events will be generated when another actor overlaps.
	//! @return true if the actor is a sensor
	bool IsSensor();

	//! test a point for overlap 
	bool IsPointInActor(const DOUBLE2 &pointRef);

	//! cast a ray at a fixture to get the point of first intersection and normal vector. No hit will register
	//! if the ray starts inside the fixture.
	//! returns true if an intersection point was found
	//! @param point1 first point of the ray
	//! @param point2 second point of the ray
	//! @param intersectionRef resulting intersection point
	//! @param normalRef resulting normal on the surface of intersection point
	//! @param fractionRef resulting fraction of the intersection is a value between 0 and 1 where
	//!        0 -> intersection at point1 
	//!        1 -> intersection at point2
	bool Raycast(DOUBLE2 point1, DOUBLE2 point2, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef);

	//! forces act gradually over time to change the velocity of a body. Use this with the KeyboardKeyDown.
	//! @param force is the force vector that is to be applied on the actor
	//! @param offsetPoint is the point at which we apply the force/impulse in object space (according to the position of the physics actor) 
	void ApplyForce(DOUBLE2 force, DOUBLE2 offsetPoint=DOUBLE2());

	//! forces act gradually over time to change the velocity of a body. Use this with the KeyboardKeyDown.
	//! @param torque is the torque that is to be applied on the actor
	void ApplyTorque(double torque);

	//! impulses can change a body's velocity immediately. Use Impulse with KeyBoardKeyPressed.
	//! @param force is the force vector that is to be applied on the actor
	//! @param offsetPoint is the point at which we apply the force/impulse in object space (according to the position of the physics actor) 
	void ApplyLinearImpulse(DOUBLE2 impulse, DOUBLE2 offsetPoint = DOUBLE2());

	//! impulses can change a body's velocity immediately. Use Impulse with KeyBoardKeyPressed.
	//! @param impulse is the impulse that is to be applied on the actor
	void ApplyAngularImpulse(double impulse);

	//! if one of this actors fixtures overlaps with one of the other actor this will return true
	//! @param actOther is the pointer of the other actor object
	bool IsOverlapping(PhysicsActor* actOther);

	//! retrieve a list of all the touching contact points
	//! @return a std::vector containing all the touching contact points
	std::vector<DOUBLE2> GetContactList();

	//! add this object to the contactlistener. This must be an object of a class that is derived from the ContactListener Class
	//! contacts with this actor will be reported in BeginContact or EndContact
	//! @param listenerPtr is "this": the pointer of the object derived from the contactlistener class
	void AddContactListener(ContactListener *listenerPtr);

	//! remove this actor object from the contactlistener. 
	//! contacts with this actor will no longer be reported in BeginContact or EndContact
	void RemoveContactListener();

	//! return the ContactListener pointer that was stored through AddContactListener. This can be a nullptr.
	ContactListener *GetContactListener()  const;

	//! Sets the category for a collision filtering
	//! see http://www.iforce2d.net/b2dtut/collision-filtering
	void SetCollisionFilter(const b2Filter &filterRef);

	//! returns the collision filter
	//! see http://www.iforce2d.net/b2dtut/collision-filtering
	b2Filter GetCollisionFilter() const;

	// Internal use. Box2D has constrains to dimensions of objects, pixels are not meters
	static const int SCALE = 100;

private:
	//private access specifier at the bottom of the file
	void Extract(int resourceID, const String& typeRef, String &resultingFilenameRef);

	// apply the state of m_bSensor to this fixture
	void ApplySensorToFixture(b2Fixture * fixturePtr);

	//! we have a lot of friends here
	friend class PhysicsRevoluteJoint;
	friend class PhysicsPrismaticJoint;
	friend class PhysicsDistanceJoint;
	friend class PhysicsWeldJoint;

	//! private internal member function, not for students
	bool CreateBody(DOUBLE2 pos, double angle, BodyType bodyType);

	//! is the actor a sensor?
	bool m_bSensor = false;

	//! userdata for eg id purposes
	int m_UserData = 0;
	void *m_UserPtr = nullptr;
	
	//! (rigid) body: A chunk of matter that is so strong that the distance between any two bits of matter on the chunk is constant. They are hard like a diamond. 
	b2Body* m_BodyPtr = nullptr;

	//! filterdata is part of each fixture
	b2Filter m_CollisionFilter;
};

