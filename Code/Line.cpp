// Line.cpp

#include "Line.h"
#include "LineSegment.h"

using namespace _3DMath;

Line::Line( void )
{
}

Line::Line( const Vector& center, const Vector& normal )
{
	this->center = center;
	this->normal = normal;
}

Line::Line( const LineSegment& lineSegment )
{
	center = lineSegment.vertex[0];
	normal.Subtract( lineSegment.vertex[1], lineSegment.vertex[0] );
	normal.Normalize();
}

Line::~Line( void )
{
}

double Line::ShortestDistance( const Vector& point ) const
{
	Vector vec;
	vec.Subtract( point, center );
	double dot = vec.Dot( normal );
	double distance = sqrt( vec.Dot( vec ) - dot * dot );
	return distance;
}

// Line.cpp