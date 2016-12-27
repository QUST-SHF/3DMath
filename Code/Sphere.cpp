// Sphere.cpp

#include "Sphere.h"
#include "LineSegment.h"
#include "AxisAlignedBox.h"

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

double Sphere::Distance( const Vector& point ) const
{
	return point.Distance( center ) - radius;
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
	Vector diff;
	diff.Subtract( point, center );

	double B = 2.0 * diff.Dot( unitVector );
	double C = diff.Dot( diff ) - radius * radius;
	double descriminant = B * B - 4.0 * C;
	if( descriminant < 0.0 )
		return 0;

	if( descriminant == 0.0 )
	{
		double lambda = -B / 2.0;
		intersectionPoints[0].AddScale( center, 1.0, unitVector, lambda );
		return 1;
	}

	double lambda = ( -B + sqrt( descriminant ) ) / 2.0;
	intersectionPoints[0].AddScale( center, 1.0, unitVector, lambda );

	lambda = ( -B - sqrt( descriminant ) ) / 2.0;
	intersectionPoints[1].AddScale( center, 1.0, unitVector, lambda );

	return 2;
}

int Sphere::Intersect( const LineSegment& lineSegment, Vector* intersectionPoints ) const
{
	Vector unitVector;
	unitVector.Subtract( lineSegment.vertex[1], lineSegment.vertex[0] );
	unitVector.Normalize();

	Vector rayIntersectionPoints[2];
	int rayCount = RayCast( lineSegment.vertex[0], unitVector, rayIntersectionPoints );

	int count = 0;

	for( int i = 0; i < rayCount; i++ )
	{
		double lambda;
		lineSegment.LerpInverse( lambda, rayIntersectionPoints[i] );

		if( AxisAlignedBox::InInterval( 0.0, 1.0, lambda, 0.0 ) )
			intersectionPoints[ count++ ] = rayIntersectionPoints[i];
	}

	return count;
}

// Sphere.cpp