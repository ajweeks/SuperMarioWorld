//-----------------------------------------------------------------
// Game Engine
// C++ Header - version v2_18a march 2016 
// Copyright Kevin Hoefman, 2006 - 2011 
// Copyright DAE Programming Team, 2012 - 2016 
// http://www.digitalartsandentertainment.be/ 
//-----------------------------------------------------------------

#pragma once

class Box2DDebugRenderer : public b2Draw
{
public:
	virtual ~Box2DDebugRenderer();

	// C++11 make the class non-copyable
	Box2DDebugRenderer(const Box2DDebugRenderer&) = delete;
	Box2DDebugRenderer& operator=(const Box2DDebugRenderer&) = delete;

	// Draw a closed polygon provided in CCW order.
	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	// Draw a solid closed polygon provided in CCW order.
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	// Draw a circle.
	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);

	// Draw a solid circle.
	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);

	// Draw a line segment.
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

	// Draw a transform. Choose your own length scale.
	void DrawTransform(const b2Transform& xf);

	void Paint();

private:
	friend class GameEngine;
	Box2DDebugRenderer();

	COLOR m_Color;
	MATRIX3X2 m_MatScale;
};
