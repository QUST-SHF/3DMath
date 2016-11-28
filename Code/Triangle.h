// Triangle.h

#pragma once

#include "Defines.h"
#include "Vector.h"

namespace _3DMath
{
	class Triangle;
	class Plane;
	class LineSegment;
}

class _3DMATH_API _3DMath::Triangle
{
public:

	Triangle( void );
	Triangle( const Vector& vertex0, const Vector& vertex1, const Vector& vertex2 );
	virtual ~Triangle( void );

	double Area( void ) const;
	void GetNormal( Vector& normal, int index ) const;
	void GetPlane( Plane& plane ) const;
	bool ContainsPoint( const Vector& point, double eps = EPSILON ) const;
	bool IsDegenerate( double eps = EPSILON ) const;
	bool Intersect( const LineSegment& lineSegment, Vector& intersectionPoint ) const;

	Vector vertex[3];
};

// Triangle.h
