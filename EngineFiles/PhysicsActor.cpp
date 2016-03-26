//-----------------------------------------------------------------
// Game Engine
// C++ Source - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------
#include "stdafx.h" //for compiler
#include "../stdafx.h" // for intellisense

#include "PhysicsActor.h"


// http://www.iforce2d.net/b2dtut
// http://box2d.org/manual.pdf

PhysicsActor::PhysicsActor(DOUBLE2 pos, double angle, BodyType bodyType)
{
	CreateBody(pos, angle, bodyType);
}
PhysicsActor::PhysicsActor(double posX, double posY, double angle, BodyType bodyType)
{
	CreateBody(DOUBLE2(posX, posY), angle, bodyType);
}

PhysicsActor::~PhysicsActor()
{
	// no more need to call RemoveActionListener before deleting a PhysicsActor object
	for (b2Fixture* fixturePtr = m_BodyPtr->GetFixtureList(); fixturePtr != nullptr; fixturePtr = fixturePtr->GetNext())
	{
		fixturePtr->SetUserData(nullptr);
	}

	// remove the body from the scene
	(GameEngine::GetSingleton())->GetBox2DWorld()->DestroyBody(m_BodyPtr);
	// from here, there can be a jump to GameEngine::EndContact !!
	m_BodyPtr = nullptr;
}

b2Body* PhysicsActor::GetBody()
{
	return m_BodyPtr;
}

void PhysicsActor::SetUserData(int data)
{
	m_UserData = data;
}

int PhysicsActor::GetUserData() const
{
	return m_UserData;
}

void PhysicsActor::SetUserPointer(void * ptr)
{
	m_UserPtr = ptr;
}

void * PhysicsActor::GetUserPointer()
{
	return m_UserPtr;
}

bool PhysicsActor::CreateBody(DOUBLE2 pos, double angle, BodyType bodyType)
{
	b2BodyDef bodyDef;
	// Define the dynamic body. We set its position and call the body factory.
	switch (bodyType)
	{
	case BodyType::DYNAMIC:
		bodyDef.type = b2_dynamicBody;
		break;
	case BodyType::STATIC:
		bodyDef.type = b2_staticBody;
		break;
	case BodyType::KINEMATIC:
		bodyDef.type = b2_kinematicBody;
		break;

	}

	// rescale to Box2D coordinates
	pos /= SCALE;

	bodyDef.position.Set((float)(pos.x), (float)(pos.y));
	bodyDef.angle = (float)angle;

	m_BodyPtr = (GameEngine::GetSingleton())->GetBox2DWorld()->CreateBody(&bodyDef);

	if (m_BodyPtr == nullptr) return false;
	return true;
}

b2Fixture* PhysicsActor::AddBoxFixture(double width, double height, double restitution, double friction, double density)
{
	b2FixtureDef fixtureDef;
	width /= SCALE; height /= SCALE;

	// The extents are the half-widths of the box.
	b2PolygonShape shape;
	shape.SetAsBox((float)width / 2, (float)height / 2);

	// Define the dynamic body fixture.	
	fixtureDef.shape = &shape;

	fixtureDef.restitution = (float)restitution;
	// Set the box density to be non-zero, so it will be dynamic.
	if (m_BodyPtr->GetType() == b2_dynamicBody)fixtureDef.density = (float)density;
	else fixtureDef.density = 0.0f;

	// Override the default friction.
	fixtureDef.friction = (float)friction;

	// store this for contact information
	fixtureDef.userData = (void *) this;

	// set the collision filter
	fixtureDef.filter = m_CollisionFilter;

	// Add the shape to the body.
	b2Fixture *fixturePtr = m_BodyPtr->CreateFixture(&fixtureDef);
	if (fixturePtr != nullptr)
	{
		ApplySensorToFixture(fixturePtr);
	}
	return fixturePtr;
}

b2Fixture* PhysicsActor::AddCircleFixture(double radius, DOUBLE2 offset, double restitution, double friction, double density)
{
	b2FixtureDef fixtureDef;
	radius /= SCALE;
	// The extents are the half-widths of the box.
	b2CircleShape circle;
	// position, relative to body position
	circle.m_p.Set((float)offset.x / SCALE, (float)offset.y / SCALE);
	// radius 
	circle.m_radius = (float)radius;

	// Define the dynamic body fixture.	
	fixtureDef.shape = &circle;

	fixtureDef.restitution = (float)restitution;
	// Set the box density to be non-zero, so it will be dynamic.
	if (m_BodyPtr->GetType() == b2_dynamicBody)fixtureDef.density = (float)density;
	else fixtureDef.density = 0.0f;

	// Override the default friction.
	fixtureDef.friction = (float)friction;

	// store this for contact information
	fixtureDef.userData = (void *) this;

	// set the collision filter
	fixtureDef.filter = m_CollisionFilter;

	// Add the shape to the body.
	b2Fixture *fixturePtr = m_BodyPtr->CreateFixture(&fixtureDef);
	if (fixturePtr != nullptr)
	{
		ApplySensorToFixture(fixturePtr);
	}
	return fixturePtr;
}

b2Fixture* PhysicsActor::AddPolygonFixture(const std::vector<DOUBLE2>& vertexArrRef, double restitution, double friction, double density)
{
	b2FixtureDef fixtureDef;
	std::vector<b2Vec2> vecArr;
	// scale to phyics units
	for (size_t i = 0; i < vertexArrRef.size(); i++)
	{
		vecArr.push_back(b2Vec2((float)vertexArrRef[i].x / SCALE, (float)vertexArrRef[i].y / SCALE));
	}

	//pass array to the shape
	b2PolygonShape polygonShape;
	polygonShape.Set(vecArr.data(), vecArr.size());

	// Define the dynamic body fixture.	
	fixtureDef.shape = &polygonShape;

	fixtureDef.restitution = (float)restitution;
	// Set the box density to be non-zero, so it will be dynamic.
	if (m_BodyPtr->GetType() == b2_dynamicBody)fixtureDef.density = (float)density;
	else fixtureDef.density = 0.0f;

	// Override the default friction.
	fixtureDef.friction = (float)friction;

	// store this for contact information
	fixtureDef.userData = (void *) this;

	// set the collision filter
	fixtureDef.filter = m_CollisionFilter;

	// Add the shape to the body.
	b2Fixture *fixturePtr = m_BodyPtr->CreateFixture(&fixtureDef);
	if (fixturePtr != nullptr)
	{
		ApplySensorToFixture(fixturePtr);
	}
	return fixturePtr;
}

b2Fixture* PhysicsActor::AddChainFixture(const std::vector<DOUBLE2>& vertexArrRef, bool closed, double restitution, double friction, double density)
{
	b2FixtureDef fixtureDef;
	std::vector<b2Vec2> vecArr;

	// seems that svg start vectex and end vertex are sometimes too close together or coincide, 
	// causing a crash in Box2D -> remove the last element
	// check the distance between begin and end points, omit the end point
	int omitLastVertex = 0;
	if ((vertexArrRef[0] - vertexArrRef[vertexArrRef.size() - 1]).SquaredLength() < 0.1)
	{
		omitLastVertex = 1;
	}

	for (size_t i = 0; i < vertexArrRef.size() - omitLastVertex; i++)
	{
		vecArr.push_back(b2Vec2((float)vertexArrRef[i].x / SCALE, (float)vertexArrRef[i].y / SCALE));
	}

	//pass array to the shape
	b2ChainShape chainShape;
	if (closed)chainShape.CreateLoop(vecArr.data(), vecArr.size());
	else chainShape.CreateChain(vecArr.data(), vecArr.size());

	// Define the dynamic body fixture.	
	fixtureDef.shape = &chainShape;

	fixtureDef.restitution = (float)restitution;
	// Set the box density to be non-zero, so it will be dynamic.
	if (m_BodyPtr->GetType() == b2_dynamicBody)fixtureDef.density = (float)density;
	else fixtureDef.density = 0.0f;

	// Override the default friction.
	fixtureDef.friction = (float)friction;

	// store this for contact information
	fixtureDef.userData = (void *) this;

	// set the collision filter
	fixtureDef.filter = m_CollisionFilter;

	// Add the shape to the body.
	b2Fixture *fixturePtr = m_BodyPtr->CreateFixture(&fixtureDef);
	if (fixturePtr != nullptr)
	{
		ApplySensorToFixture(fixturePtr);
	}
	return fixturePtr;
}

std::vector<b2Fixture *> PhysicsActor::AddSVGFixture(BYTE* pBlob, int blobSize, double restitution, double friction, double density)
{
	// a vector containing chains
	std::vector<std::vector<DOUBLE2>> verticesArr;

	//parse the svg file
	SVGParser svgParser;
	svgParser.LoadGeometryFromSvgStream(pBlob, blobSize, verticesArr);

	std::vector<b2Fixture *>fixPtrArr;
	// process the chains
	for (size_t i = 0; i < verticesArr.size(); i++)
	{
		std::vector<DOUBLE2> &chain = verticesArr[i];
		b2Fixture * fixPtr = AddChainFixture(chain, true, restitution, friction, density);
		if (fixPtr != nullptr)
		{
			fixPtrArr.push_back(fixPtr);
		}
		else
		{
			OutputDebugStringA("svg Chain creation failed");
		}
	}
	return fixPtrArr;
}

std::vector<b2Fixture *> PhysicsActor::AddSVGFixture(const String & svgFilePathRef, double restitution, double friction, double density)
{
	// a vector containing chains
	std::vector<std::vector<DOUBLE2>> verticesArr;

	//parse the svg file
	SVGParser svgParser;
	svgParser.LoadGeometryFromSvgFile(svgFilePathRef, verticesArr);

	std::vector<b2Fixture *>fixPtrArr;
	// process the chains
	for (size_t i = 0; i < verticesArr.size(); i++)
	{
		std::vector<DOUBLE2> &chain = verticesArr[i];
		b2Fixture * fixPtr = AddChainFixture(chain, true, restitution, friction, density);
		if (fixPtr != nullptr)
		{
			fixPtrArr.push_back(fixPtr);
		}
		else
		{
			OutputDebugStringA("svg Chain creation failed");
		}
	}
	return fixPtrArr;
}

// add these definition to the PhysicsActor.cpp file:
std::vector<b2Fixture *> PhysicsActor::AddSVGFixture(int resourceID, double restitution, double friction, double density)
{
	String sType("SVG");
	String fileName = String(resourceID) + String(".") + sType;
	String resultingFilename;
	Extract(resourceID, sType, resultingFilename);
	return AddSVGFixture(resultingFilename, restitution, friction, density);
}

void PhysicsActor::Extract(int resourceID, const String& typeRef, String &resultingFilenameRef)
{
	HRSRC hrsrc = FindResourceA(NULL, MAKEINTRESOURCEA(resourceID), typeRef.C_str());
	HGLOBAL hLoaded = LoadResource(NULL, hrsrc);
	LPVOID lpLock = LockResource(hLoaded);
	DWORD dwSize = SizeofResource(NULL, hrsrc);

	String path("temp/");
	CreateDirectoryA(path.C_str(), NULL);

	resultingFilenameRef = path + String(resourceID) + String(".") + typeRef;
	HANDLE hFile = CreateFileA(resultingFilenameRef.C_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	DWORD dwByteWritten = 0;
	WriteFile(hFile, lpLock, dwSize, &dwByteWritten, NULL);
	CloseHandle(hFile);
	FreeResource(hLoaded);
}

void PhysicsActor::DestroyFixture(b2Fixture *fixturePtr)
{
	if(fixturePtr != nullptr) m_BodyPtr->DestroyFixture(fixturePtr);
}

DOUBLE2 PhysicsActor::GetPosition() const
{
	b2Vec2 position = m_BodyPtr->GetPosition();
	return DOUBLE2(position.x, position.y) * SCALE;
}

void PhysicsActor::SetPosition(double posX, double posY)
{
	m_BodyPtr->SetTransform(b2Vec2((float)(posX / SCALE), (float)(posY / SCALE)), m_BodyPtr->GetAngle());
	m_BodyPtr->SetAwake(true);
}
void PhysicsActor::SetPosition(const DOUBLE2& positionRef)
{
	m_BodyPtr->SetTransform(b2Vec2((float)(positionRef.x / SCALE), (float)(positionRef.y / SCALE)), m_BodyPtr->GetAngle());
	m_BodyPtr->SetAwake(true);
}

double PhysicsActor::GetAngle() const
{
	float32 angle = m_BodyPtr->GetAngle(); 
	return angle;
}

void PhysicsActor::SetAngle(double angle)
{
	m_BodyPtr->SetTransform(m_BodyPtr->GetPosition(), (float)angle);
	m_BodyPtr->SetAwake(true);
}

void PhysicsActor::SetLinearVelocity(DOUBLE2 velocity)
{
	velocity /= SCALE;
	//if (m_BodyPtr->GetType() != b2_dynamicBody) 
	//{
		
		m_BodyPtr->SetLinearVelocity(b2Vec2((float)velocity.x, (float)velocity.y));
		m_BodyPtr->SetAwake(true);
	//}
	//// dynamic body uses mass to calculate needed impulse
	//else//does not work as espected
	//{
	//	b2Vec2 targetVelocity((float32)velocity.x, (float32)velocity.y) ;
	//	b2Vec2 currentVelocity = m_BodyPtr->GetLinearVelocity();
	//	b2Vec2 velocityChange = targetVelocity - currentVelocity;
	//	b2Vec2 impulse = m_BodyPtr->GetMass() * velocityChange;
	//	m_BodyPtr->ApplyLinearImpulse(impulse, b2Vec2(), true);
	//}
}

DOUBLE2  PhysicsActor::GetLinearVelocity() const
{
	b2Vec2 v = m_BodyPtr->GetLinearVelocity();
	return DOUBLE2(v.x, v.y) * SCALE;
}

void PhysicsActor::SetAngularVelocity(double velocity)
{
 	m_BodyPtr->SetAngularVelocity((float)velocity);
	m_BodyPtr->SetAwake(true);
}

double  PhysicsActor::GetAngularVelocity() const
{
	return m_BodyPtr->GetAngularVelocity(); 
}

double PhysicsActor::GetMass() const
{
	return m_BodyPtr->GetMass() * SCALE * SCALE;
}

void PhysicsActor::SetFixedRotation(bool fixedRotation)
{
	m_BodyPtr->SetFixedRotation(fixedRotation);
}

bool PhysicsActor::GetFixedRotation() const
{
	return m_BodyPtr->IsFixedRotation();
}

void PhysicsActor::SetBullet(bool bullet)
{
	m_BodyPtr->SetBullet(bullet);
}

bool PhysicsActor::GetBullet()
{
	return m_BodyPtr->IsBullet();
}

void PhysicsActor::SetActive(bool active)
{
	m_BodyPtr->SetActive(active);
}

bool PhysicsActor::IsActive()
{
	return m_BodyPtr->IsActive();
}

void PhysicsActor::SetGravityScale(double scale)
{
	m_BodyPtr->SetGravityScale((float)scale);
}

double PhysicsActor::GetGravityScale()
{
	return (float)m_BodyPtr->GetGravityScale();
}

void PhysicsActor::SetBodyType(BodyType bodyType)
{
	switch (bodyType)
	{
	case BodyType::DYNAMIC:
		if (m_BodyPtr->GetType() != b2BodyType::b2_dynamicBody)
		{
			m_BodyPtr->SetType(b2BodyType::b2_dynamicBody);
		}
		break;
	case BodyType::STATIC:
		if (m_BodyPtr->GetType() != b2BodyType::b2_staticBody)
		{
			m_BodyPtr->SetType(b2BodyType::b2_staticBody);
		}
		break;
	case BodyType::KINEMATIC:
		if (m_BodyPtr->GetType() != b2BodyType::b2_kinematicBody)
		{
			m_BodyPtr->SetType(b2BodyType::b2_kinematicBody);
		}
		break;
	}
}

BodyType PhysicsActor::GetBodyType()
{
	switch (m_BodyPtr->GetType())
	{
	case b2BodyType::b2_dynamicBody:
		return BodyType::DYNAMIC;
	case b2BodyType::b2_staticBody:
		return BodyType::STATIC;
	case b2BodyType::b2_kinematicBody:
		return BodyType::KINEMATIC;
	default: return BodyType::STATIC;
	}	
}

void PhysicsActor::SetAwake(bool flag)
{
	m_BodyPtr->SetAwake(flag);
}

bool PhysicsActor::IsAwake() const
{
	return m_BodyPtr->IsAwake();
}

void PhysicsActor::SetSensor(bool isSensor)
{
	m_bSensor = isSensor;
	for (b2Fixture* fixturePtr = m_BodyPtr->GetFixtureList(); fixturePtr != nullptr; fixturePtr = fixturePtr->GetNext())
	{
		ApplySensorToFixture(fixturePtr);
	}
}

bool PhysicsActor::IsSensor()
{
	return m_bSensor;
}

bool PhysicsActor::IsPointInActor(const DOUBLE2 &pointRef)
{
	bool hit = false;
	for (b2Fixture* fixturePtr = m_BodyPtr->GetFixtureList(); fixturePtr != nullptr && !hit; fixturePtr = fixturePtr->GetNext())
	{
		hit = fixturePtr->TestPoint(b2Vec2((float)pointRef.x / SCALE, (float)pointRef.y / SCALE));
	}
	return hit;
}

bool PhysicsActor::Raycast(DOUBLE2 point1, DOUBLE2 point2, DOUBLE2 &intersectionRef, DOUBLE2 &normalRef, double &fractionRef)
{
	point1 /= SCALE;
	point2 /= SCALE;
	b2Transform transform;
	//transform.SetIdentity();
	transform.Set(m_BodyPtr->GetPosition(), m_BodyPtr->GetAngle());
	b2RayCastInput input;
	input.p1.Set((float)point1.x, (float)point1.y);
	input.p2.Set((float)point2.x, (float)point2.y);
	input.maxFraction = 1.0f;
	b2RayCastOutput output, closestOutput;
	closestOutput.fraction = 1; //start with end of line as p2
	//check every fixture of every body to find closest 
	for (b2Fixture* fixturePtr = m_BodyPtr->GetFixtureList(); fixturePtr != nullptr; fixturePtr = fixturePtr->GetNext())
	{
		//A child index is included for chain shapes because the ray cast will only	check a single edge at a time.
		for (int edgeIndex = 0; edgeIndex < fixturePtr->GetShape()->GetChildCount(); ++edgeIndex)
		{
			bool hit = fixturePtr->GetShape()->RayCast(&output, input, transform, edgeIndex);
			if (hit)
			{
				if (output.fraction < closestOutput.fraction)
					closestOutput = output;
			}
		}
	}
	if (closestOutput.fraction < 1)
	{
		b2Vec2 hitPoint = input.p1 + closestOutput.fraction * (input.p2 - input.p1);
		intersectionRef = DOUBLE2(hitPoint.x, hitPoint.y) * SCALE;
		normalRef = DOUBLE2(output.normal.x, output.normal.y);
		fractionRef = output.fraction;
		return true;
	}
	// no intersection
	return false;
}

void PhysicsActor::ApplyForce(DOUBLE2 force, DOUBLE2 offsetPoint)
{
	force /= (SCALE * SCALE);
	if (offsetPoint.x == 0 && offsetPoint.y == 0)
	{
		m_BodyPtr->ApplyForceToCenter(b2Vec2((float)force.x, (float)force.y), true);
	}
	else
	{
		b2Vec2 p = m_BodyPtr->GetWorldPoint(b2Vec2((float)offsetPoint.x / SCALE, (float)offsetPoint.y / SCALE));
		m_BodyPtr->ApplyForce(b2Vec2((float)force.x, (float)force.y), p, true);
	}
}

void PhysicsActor::ApplyTorque(double torque)
{
	torque /= (SCALE * SCALE);
	m_BodyPtr->ApplyTorque((float)torque, true);
}

void PhysicsActor::ApplyLinearImpulse(DOUBLE2 impulse, DOUBLE2 offsetPoint)
{
	impulse /= (SCALE * SCALE);
	b2Vec2 p = m_BodyPtr->GetWorldPoint(b2Vec2((float)offsetPoint.x / SCALE, (float)offsetPoint.y / SCALE));
	m_BodyPtr->ApplyLinearImpulse(b2Vec2((float)impulse.x, (float)impulse.y), p, true);
}

void PhysicsActor::ApplyAngularImpulse(double impulse)
{
	impulse /= (SCALE * SCALE);
	m_BodyPtr->ApplyAngularImpulse((float)impulse, true);
}

void PhysicsActor::ApplySensorToFixture(b2Fixture * fixturePtr)
{
	fixturePtr->SetSensor(m_bSensor);
}

bool PhysicsActor::IsOverlapping(PhysicsActor* actOther)
{
	b2Transform transform;
	transform.Set(m_BodyPtr->GetPosition(), m_BodyPtr->GetAngle());
	b2Transform otherTransform;
	otherTransform.Set(actOther->m_BodyPtr->GetPosition(), actOther->m_BodyPtr->GetAngle());

	for (b2Fixture* fixturePtr = m_BodyPtr->GetFixtureList(); fixturePtr != nullptr; fixturePtr = fixturePtr->GetNext())
	{
		for (int edgeIndex = 0; edgeIndex < fixturePtr->GetShape()->GetChildCount(); ++edgeIndex)
		{
			for (b2Fixture* otherfixturePtr = actOther->m_BodyPtr->GetFixtureList(); otherfixturePtr != nullptr; otherfixturePtr = otherfixturePtr->GetNext())
			{
				for (int otherEdgeIndex = 0; otherEdgeIndex < otherfixturePtr->GetShape()->GetChildCount(); ++otherEdgeIndex)
				{
					if (b2TestOverlap(fixturePtr->GetShape(), edgeIndex, otherfixturePtr->GetShape(), otherEdgeIndex, transform, otherTransform))
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

std::vector<DOUBLE2> PhysicsActor::GetContactList()
{
	std :: vector<DOUBLE2> contactPoints;
	for (b2ContactEdge* edgePtr = m_BodyPtr->GetContactList(); edgePtr; edgePtr = edgePtr->next)
	{
		if (edgePtr->contact->IsTouching())
		{
			//...world manifold is helpful for getting locations
			b2WorldManifold worldManifold;
			edgePtr->contact->GetWorldManifold(&worldManifold);

			int numPoints = edgePtr->contact->GetManifold()->pointCount;

			for (int i = 0; i < numPoints; i++)
			{
				contactPoints.push_back(DOUBLE2(worldManifold.points[i].x * SCALE, worldManifold.points[i].y * SCALE));
			}
		}
	}
	return contactPoints;
}

void PhysicsActor::AddContactListener(ContactListener *listenerPtr)
{
	//store the pointer in userdata to be used by the ContactCaller
	m_BodyPtr->SetUserData((void*)listenerPtr);
}

void PhysicsActor::RemoveContactListener()
{
	//reset the pointer in userdata to be used by the ContactCaller
	m_BodyPtr->SetUserData((void*)nullptr);
}

ContactListener *PhysicsActor::GetContactListener() const
{
	return reinterpret_cast <ContactListener *>(m_BodyPtr->GetUserData());
}

void PhysicsActor::SetCollisionFilter(const b2Filter &filterRef)
{
	m_CollisionFilter = filterRef;
	//apply on all fixtures
	for (b2Fixture* fixturePtr = m_BodyPtr->GetFixtureList(); fixturePtr != nullptr; fixturePtr = fixturePtr->GetNext())
	{
		fixturePtr->SetFilterData(m_CollisionFilter);
	}
}

b2Filter PhysicsActor::GetCollisionFilter() const
{
	return m_CollisionFilter;
}