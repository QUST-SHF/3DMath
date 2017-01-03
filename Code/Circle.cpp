// Circle.cpp

#include "Circle.h"
#include "Plane.h"
#include "Sphere.h"

using namespace _3DMath;

Circle::Circle( void )
{
	radius = 0.0;
	center.Set( 0.0, 0.0, 0.0 );
	normal.Set( 0.0, 0.0, 1.0 );
}

Circle::Circle( const Vector& center, const Vector& normal, double radius )
{
	this->center = center;
	normal.GetNormalized( this->normal );
	this->radius = radius;
}

Circle::~Circle( void )
{
}

bool Circle::SetAsIntersectionOf( const Sphere& sphereA, const Sphere& sphereB )
{
	return false;
}

bool Circle::SetAsIntersectionOf( const Plane& plane, const Sphere& sphere )
{
	center = sphere.center;
	plane.NearestPoint( center );

	normal = plane.normal;

	Vector diff;
	diff.Subtract( sphere.center, center );

	double squareRadius = sphere.radius * sphere.radius - diff.Dot( diff );
	if( squareRadius < 0.0 )
		return false;

	radius = sqrt( squareRadius );
	return true;
}

// Circle.cpp