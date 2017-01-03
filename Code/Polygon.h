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
	class AffineTransform;

	typedef std::list< Polygon* > PolygonList;
}

class _3DMATH_API _3DMath::Polygon
{
public:

	Polygon( void );
	virtual ~Polygon( void );

	bool SplitAgainstSurface( const Surface* surface, PolygonList& polygonList, double maxDistanceFromSurface ) const;
	bool Tessellate( void ) const;
	bool GetPlane( Plane& plane ) const;
	void GetCenter( Vector& center ) const;
	void GetIntegratedCenter( Vector& center, double delta ) const;
	bool GetTriangleAverageCenter( Vector& center ) const;
	void Transform( const AffineTransform& transform );

	// This is a list of points in 3D space presumed to be coplanar,
	// and forming a polyline loop without any self-intersection.  It
	// may be convex or concave.
	VectorArray* vertexArray;
	IndexTriangleList* indexTriangleList;
};

// Polygon.h
