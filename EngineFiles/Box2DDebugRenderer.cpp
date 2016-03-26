//-----------------------------------------------------------------
// Game Engine
// C++ Source - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/
//-----------------------------------------------------------------
#include "stdafx.h"
#include "../stdafx.h"

#include "Box2DDebugRenderer.h"


Box2DDebugRenderer::Box2DDebugRenderer()
{
	m_Color = COLOR(255, 255, 255, 187);
	m_MatScale = MATRIX3X2::CreateScalingMatrix(PhysicsActor::SCALE);
}

Box2DDebugRenderer::~Box2DDebugRenderer()
{

}

//------------------------------------------------------------------------------
// Box2DDebugRenderer class definitions. Draws Box2D debug information
//------------------------------------------------------------------------------

/// Draw a closed polygon provided in CCW order.
void Box2DDebugRenderer::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	std::vector<DOUBLE2> vertexsArr;
	for (int i = 0; i < vertexCount; i++)
	{
		vertexsArr.push_back(DOUBLE2(vertices[i].x, vertices[i].y));
	}
	GameEngine::GetSingleton()->SetWorldMatrix(m_MatScale);
	GameEngine::GetSingleton()->SetColor(COLOR((byte)(color.r * 255), (byte)(color.g * 255), (byte)(color.b * 255) , (byte)(color.a * 120)));
	GameEngine::GetSingleton()->DrawPolygon(vertexsArr, true, 1 / PhysicsActor::SCALE);
}

//------------------------------------------------------------------------------
// Box2DDebugRenderer class definitions. Encapsulated the user defined game settings
//------------------------------------------------------------------------------
/// Draw a solid closed polygon provided in CCW order.
void Box2DDebugRenderer::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	std::vector<DOUBLE2> vertexsArr;
	for (int i = 0; i < vertexCount; i++)
	{
		vertexsArr.push_back(DOUBLE2(vertices[i].x, vertices[i].y));
	}
	GameEngine::GetSingleton()->SetWorldMatrix(m_MatScale);
	GameEngine::GetSingleton()->SetColor(COLOR((byte)(color.r * 255), (byte)(color.g * 255), (byte)(color.b * 255), (byte)(color.a * 120)));
	GameEngine::GetSingleton()->FillPolygon(vertexsArr);
}

/// Draw a circle.
void Box2DDebugRenderer::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	GameEngine::GetSingleton()->SetWorldMatrix(m_MatScale);
	GameEngine::GetSingleton()->SetColor(COLOR((byte)(color.r * 255), (byte)(color.g * 255), (byte)(color.b * 255), (byte)(color.a * 120)));
	GameEngine::GetSingleton()->DrawEllipse(DOUBLE2(center.x, center.y), radius, radius, (double)1 / PhysicsActor::SCALE);
}

/// Draw a solid circle.
void Box2DDebugRenderer::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	GameEngine::GetSingleton()->SetWorldMatrix(m_MatScale);
	GameEngine::GetSingleton()->SetColor(COLOR((byte)(color.r * 255), (byte)(color.g * 255), (byte)(color.b * 255), (byte)(color.a * 120)));
	GameEngine::GetSingleton()->FillEllipse(DOUBLE2(center.x, center.y), radius, radius);
}

/// Draw a line segment.
void Box2DDebugRenderer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	GameEngine::GetSingleton()->SetWorldMatrix(m_MatScale);
	GameEngine::GetSingleton()->SetColor(COLOR((byte)(color.r * 255), (byte)(color.g * 255), (byte)(color.b * 255), (byte)(color.a * 120)));
	GameEngine::GetSingleton()->DrawLine(DOUBLE2(p1.x, p1.y), DOUBLE2(p2.x, p2.y), (double)1 / PhysicsActor::SCALE);
}

/// Draw a transform. Choose your own length scale.
/// @param xf a transform.
void Box2DDebugRenderer::DrawTransform(const b2Transform& xf)
{
	GameEngine::GetSingleton()->SetWorldMatrix(m_MatScale);
	GameEngine::GetSingleton()->SetColor(m_Color);
	// the position
	GameEngine::GetSingleton()->SetColor(COLOR(0, 0, 0));
	GameEngine::GetSingleton()->DrawEllipse(DOUBLE2(xf.p.x, xf.p.y), (double)2 / PhysicsActor::SCALE, (double)2 / PhysicsActor::SCALE, (double)1 / PhysicsActor::SCALE);// DOUBLE2(p2.x, p2.y), (double)1 / PhysicsActor::SCALE);
	// the x axis
	DOUBLE2 x(xf.q.GetXAxis().x, xf.q.GetXAxis().y);
	x = x / PhysicsActor::SCALE * 20;
	GameEngine::GetSingleton()->SetColor(COLOR(255,0,0)); 
	GameEngine::GetSingleton()->DrawLine(DOUBLE2(xf.p.x, xf.p.y), DOUBLE2(xf.p.x, xf.p.y) + x, (double)1 / PhysicsActor::SCALE);

	// the y axis
	DOUBLE2 y(xf.q.GetYAxis().x, xf.q.GetYAxis().y);
	y = y / PhysicsActor::SCALE * 20;
	GameEngine::GetSingleton()->SetColor(COLOR(0,255,0)); 
	GameEngine::GetSingleton()->DrawLine(DOUBLE2(xf.p.x, xf.p.y), DOUBLE2(xf.p.x, xf.p.y) + y, (double)1 / PhysicsActor::SCALE);

}