// Sphere.cpp

#include "Sphere.h"
#include "LineSegment.h"
#include "Line.h"
#include "AxisAlignedBox.h"
#include "Function.h"

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

void Sphere::RayCast( const Line& line, VectorArray& intersectionPoints ) const
{
	RayCast( line.center, line.normal, intersectionPoints );
}

void Sphere::RayCast( const Vector& point, const Vector& unitVector, VectorArray& intersectionPoints ) const
{
	Vector diff;
	diff.Subtract( point, center );

	Quadratic quadratic;
	quadratic.A = 1.0;
	quadratic.B = 2.0 * unitVector.Dot( diff );
	quadratic.C = diff.Dot( diff ) - radius * radius;

	Quadratic::RealArray realArray;
	quadratic.FindZeros( realArray );

	intersectionPoints.clear();
	for( int i = 0; i < ( signed )realArray.size(); i++ )
	{
		double lambda = realArray[i];

		Vector pointOfIntersection;
		pointOfIntersection.AddScale( point, unitVector, lambda );

		intersectionPoints.push_back( pointOfIntersection );
	}
}

void Sphere::Intersect( const LineSegment& lineSegment, VectorArray& intersectionPoints ) const
{
	Vector unitVector;
	unitVector.Subtract( lineSegment.vertex[1], lineSegment.vertex[0] );
	unitVector.Normalize();

	VectorArray rayIntersectionPoints;
	RayCast( lineSegment.vertex[0], unitVector, rayIntersectionPoints );

	intersectionPoints.clear();

	for( int i = 0; i < ( signed )rayIntersectionPoints.size(); i++ )
	{
		double lambda;
		lineSegment.LerpInverse( lambda, rayIntersectionPoints[i] );

		if( AxisAlignedBox::InInterval( 0.0, 1.0, lambda, 0.0 ) )
			intersectionPoints.push_back( rayIntersectionPoints[i] );
	}
}

// Sphere.cpp