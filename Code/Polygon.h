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
	class LineSegment;

	typedef std::list< Polygon* > PolygonList;
}

class _3DMATH_API _3DMath::Polygon
{
public:

	Polygon( void );
	virtual ~Polygon( void );

	bool SplitAgainstSurface( const Surface* surface, PolygonList& polygonList, double minDistance, double maxDistance, double eps = EPSILON ) const;
	bool Tessellate( void ) const;
	bool GetPlane( Plane& plane ) const;
	void GetCenter( Vector& center ) const;
	bool GetTriangleAverageCenter( Vector& center ) const;
	void Transform( const AffineTransform& transform );
	double GetArea( void ) const;
	bool ContainsPoint( const Vector& point, double eps = EPSILON ) const;
	void IncreaseDensity( double minDistance );
	void MinimizeDensity( void );
	void GetCopy( Polygon& polygon ) const;
	bool GetIntersectionPoints( const LineSegment& lineSegment, VectorList& intersectionList ) const;

	// This is a list of points in 3D space presumed to be coplanar,
	// and forming a polyline loop without any self-intersection.  It
	// may be convex or concave.  It's worth pointing out that if we
	// were more suffisticated here, we would use multiple arrays (sides)
	// so that we could represent polygons from different topologies.
	// For example, we can't represent a polygon with a hole in it.  Doing
	// so, however, would complicate the algorithms beyond how far I'm willing
	// to go at the time of this writing.
	VectorArray* vertexArray;

	// This is a possible tessellation of the polygon.
	IndexTriangleList* indexTriangleList;

private:

	// This performs an arbitrary split, if any, and is not meant to be called directly.
	bool SplitInTwoAgainstSurface( const Surface* surface, Polygon*& polygonA, Polygon*& polygonB, double minDistance, double maxDistance, double eps );
	bool SplitInTwoIfNeeded( Polygon*& polygonA, Polygon*& polygonB, double eps = EPSILON );
};

// Polygon.h
