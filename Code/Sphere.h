// Sphere.h

#pragma once

#include "Defines.h"
#include "Vector.h"

namespace _3DMath
{
	class Sphere;
	class LineSegment;
	class Line;
}

class _3DMATH_API _3DMath::Sphere
{
public:

	Sphere( void );
	Sphere( const Vector& center, double radius );
	~Sphere( void );

	double Distance( const Vector& point ) const;
	bool ContainsPoint( const Vector& point ) const;
	void RayCast( const Line& line, VectorArray& intersectionPoints ) const;
	void RayCast( const Vector& point, const Vector& unitVector, VectorArray& intersectionPoints ) const;
	void Intersect( const LineSegment& lineSegment, VectorArray& intersectionPoints ) const;

	Vector center;
	double radius;
};

// Sphere.h
