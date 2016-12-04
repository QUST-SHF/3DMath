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
	double dist0 = Distance( lineSegment.vertex[0] );
	double dist1 = Distance( lineSegment.vertex[1] );
	if( ( dist0 <= 0.0 && dist1 >= 0.0 ) || ( dist1 <= 0.0 && dist0 >= 0.0 ) )
	{
		dist0 = fabs( dist0 );
		dist1 = fabs( dist1 );
		double lambda = dist0 / ( dist0 + dist1 );
		intersectionPoint.AddScale( lineSegment.vertex[0], 1.0 - lambda, lineSegment.vertex[1], lambda );
		return true;
	}
	
	return false;
}

// Plane.cpp