// Renderer.h

#pragma once

#include "Defines.h"
#include "Vector.h"
#include "TriangleMesh.h"
#include "Random.h"

namespace _3DMath
{
	class Renderer;
	class LineSegment;
	class Triangle;
	class Vector;
	class TriangleMesh;
	class Vertex;
	class ParticleSystem;
	class BoundingBoxTree;
	class AffineTransform;
	class Polygon;
	class Surface;
	class Sphere;
	class Plane;
	class Spline;
}

class _3DMATH_API _3DMath::Renderer
{
public:

	Renderer( void );
	virtual ~Renderer( void );

	enum DrawMode
	{
		DRAW_MODE_POINTS,
		DRAW_MODE_LINES,
		DRAW_MODE_TRIANGLES,
		DRAW_MODE_QUADS,
		DRAW_MODE_LINE_STRIP,
		DRAW_MODE_LINE_LOOP,
		DRAW_MODE_TRIANGLE_STRIP,
		DRAW_MODE_QUAD_STRIP,
		DRAW_MODE_TRIANGLE_FAN,
		DRAW_MODE_POLYGON,
	};

	enum DrawStyle
	{
		DRAW_STYLE_SOLID,
		DRAW_STYLE_WIRE_FRAME,
		DRAW_STYLE_VERTICES,
	};

	DrawStyle drawStyle;

	enum VertexFlags
	{
		VTX_FLAG_POSITION				= 0x00000001,
		VTX_FLAG_NORMAL					= 0x00000002,
		VTX_FLAG_COLOR					= 0x00000004,
		VTX_FLAG_TEXCOORDS				= 0x00000008,
	};

	virtual void BeginDrawMode( DrawMode drawMode ) = 0;
	virtual void EndDrawMode( void ) = 0;
	virtual void IssueVertex( const Vertex& vertex, int vertexFlags = VTX_FLAG_POSITION | VTX_FLAG_NORMAL | VTX_FLAG_COLOR | VTX_FLAG_TEXCOORDS ) = 0;
	virtual void Color( const Vector& color, double alpha = 1.0 ) = 0;
	
	enum ParticleSystemDrawFlag
	{
		DRAW_PARTICLES					= 0x00000001,
		DRAW_FORCES						= 0x00000002,
		DRAW_EMITTERS					= 0x00000004,
		DRAW_COLLISION_OBJECTS			= 0x00000008,
	};

	enum TriangleMeshDrawFlags
	{
		UV_CORRECTION					= 0x00000001,
		DRAW_NORMALS					= 0x00000002,
	};

	enum BoundingBoxTreeDrawFlags
	{
		DRAW_BOXES						= 0x00000001,
		DRAW_TRIANGLES					= 0x00000002,
	};

	void DrawVector( const Vector& vector, const Vector& position, const Vector& color, double alpha = 1.0, double arrowRadius = 1.0, int arrowSegments = 8 );
	void DrawLineSegment( const LineSegment& lineSegment );
	void DrawTriangle( const Triangle& triangle );
	void DrawTriangleMesh( const TriangleMesh& triangleMesh, int drawFlags = 0, const AffineTransform* transform = nullptr );
	void DrawParticleSystem( const ParticleSystem& particleSystem, int drawFlags = DRAW_PARTICLES );
	void DrawBoundingBoxTree( const BoundingBoxTree& boxTree, int drawFlags = DRAW_BOXES );
	void DrawPolygon( const Polygon& polygon, const AffineTransform* transform = nullptr, bool drawTessellation = true );
	void DrawSurface( const Surface& surface, const Vector& color, double alpha, const AffineTransform* transform = nullptr );
	void DrawSphere( const Sphere& sphere, const Vector& color, double alpha, const AffineTransform* transform = nullptr );
	void DrawPlane( const Plane& plane, double radius, const Vector& color, double alpha, const AffineTransform* transform = nullptr );
	void DrawDoubleFan( const _3DMath::AffineTransform& transform, double radius, const Vector* centers, int segments, const Vector& color, double alpha );
	void DrawSpline( const Spline& spline, const _3DMath::AffineTransform& transform, double maxSegmentLength );

	void CorrectUV( double texCoordAnchor, double& texCoord );

	std::set< uint64_t >* cachedEdgeSet;
	Random random;
};

// Renderer.h
