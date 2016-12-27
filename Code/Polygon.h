// Polygon.h

#pragma once

#include "Defines.h"
#include "Vector.h"
#include "IndexTriangle.h"

namespace _3DMath
{
	class Polygon;
	class Plane;
	class Sphere;

	typedef std::list< Polygon* > PolygonList;
}

class _3DMATH_API _3DMath::Polygon
{
public:

	Polygon( void );
	virtual ~Polygon( void );

	void SplitAgainstPlane( const Plane& plane, PolygonList& polygonList ) const;
	void SplitAgainstSphere( const Sphere& sphere, PolygonList& polygonList ) const;

	void Tessellate( IndexTriangleList& indexTriangleList ) const;

	// This is a list of points in 3D space presumed to be coplanar,
	// and forming a polyline loop without any self-intersection.  It
	// need not be convex; that is, it may be concave.
	VectorArray* vertexArray;
};

// Polygon.h
