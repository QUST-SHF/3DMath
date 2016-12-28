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
	class Surface;
	class Plane;

	typedef std::list< Polygon* > PolygonList;
}

class _3DMATH_API _3DMath::Polygon
{
public:

	Polygon( void );
	virtual ~Polygon( void );

	bool SplitAgainstSurface( const Surface* surface, Polygon& insidePolygon, Polygon& outsidePolygon, double maxDistanceFromSurface ) const;
	bool Tessellate( void ) const;
	bool GetPlane( Plane& plane ) const;

	// This is a list of points in 3D space presumed to be coplanar,
	// and forming a polyline loop without any self-intersection.  It
	// may be convex or concave.
	VectorArray* vertexArray;
	IndexTriangleList* indexTriangleList;
};

// Polygon.h
