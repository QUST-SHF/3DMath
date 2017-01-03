// Circle.h

#pragma once

#include "Defines.h"
#include "Vector.h"

namespace _3DMath
{
	class Circle;
	class Vector;
	class Sphere;
	class Plane;
}

class _3DMATH_API _3DMath::Circle
{
public:

	Circle( void );
	Circle( const Vector& center, const Vector& normal, double radius );
	~Circle( void );

	bool SetAsIntersectionOf( const Sphere& sphereA, const Sphere& sphereB );
	bool SetAsIntersectionOf( const Plane& plane, const Sphere& sphere );

	Vector center, normal;
	double radius;
};

// Circle.h
