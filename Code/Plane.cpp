// Plane.cpp

#include "Plane.h"
#include "LineSegment.h"

using namespace _3DMath;

Plane::Plane( void )
{
	normal.Set( 0.0, 0.0, 1.0 );
	dot = 0.0;
}

Plane::~Plane( void )
{
}

void Plane::SetCenterAndNormal( const Vector& center, const Vector& normal )
{
	normal.GetNormalized( this->normal );
	dot = -center.Dot( this->normal );
}

void Plane::GetCenter( Vector& center ) const
{
	normal.GetScaled( center, dot );
}

double Plane::Distance( const Vector& point ) const
{
	double distance = point.Dot( normal ) + dot;
	return distance;
}

Plane::Side Plane::GetSide( const Vector& point, double eps /*= EPSILON*/ ) const
{
	double distance = Distance( point );
	if( distance > eps )
		return SIDE_FRONT;
	else if( distance < -eps )
		return SIDE_BACK;
	return SIDE_NEITHER;
}

void Plane::NearestPoint( Vector& point ) const
{
	double distance = Distance( point );
	point.AddScale( normal, -distance );
}

bool Plane::Intersect( const LineSegment& lineSegment, Vector& intersectionPoint ) const
{
	double dotA = lineSegment.vertex[0].Dot( normal );
	double dotB = lineSegment.vertex[1].Dot( normal );
	double lambda = ( dot - dotA ) / ( dotB - dotA );
	if( lambda < 0.0 || lambda > 1.0 )
		return false;

	intersectionPoint.AddScale( lineSegment.vertex[0], 1.0 - lambda, lineSegment.vertex[1], lambda );
	return true;
}

// Plane.cpp