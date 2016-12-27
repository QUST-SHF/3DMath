// Sphere.cpp

#include "Sphere.h"

using namespace _3DMath;

Sphere::Sphere( void )
{
}

Sphere::Sphere( const Vector& center, double radius )
{
	this->center = center;
	this->radius = radius;
}

Sphere::~Sphere( void )
{
}

bool Sphere::ContainsPoint( const Vector& point ) const
{
	Vector diff;
	diff.Subtract( point, center );
	if( diff.Dot( diff ) <= radius * radius )
		return true;
	return false;
}

int Sphere::RayCast( const Vector& point, const Vector& unitVector, Vector* intersectionPoints ) const
{
	// TODO: Use the quadratic formula.
	return 0;
}

// Sphere.cpp