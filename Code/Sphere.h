// Sphere.h

#pragma once

#include "Defines.h"
#include "Vector.h"

namespace _3DMath
{
	class Sphere;
	class LineSegment;
}

class _3DMATH_API _3DMath::Sphere
{
public:

	Sphere( void );
	Sphere( const Vector& center, double radius );
	~Sphere( void );

	double Distance( const Vector& point ) const;
	bool ContainsPoint( const Vector& point ) const;
	int RayCast( const Vector& point, const Vector& unitVector, Vector* intersectionPoints ) const;
	int Intersect( const LineSegment& lineSegment, Vector* intersectionPoints ) const;

	Vector center;
	double radius;
};

// Sphere.h
