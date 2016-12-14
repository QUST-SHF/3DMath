// LineSegment.cpp

#include "LineSegment.h"

using namespace _3DMath;

LineSegment::LineSegment( void )
{
	vertex[0].Set( 0.0, 0.0, 0.0 );
	vertex[1].Set( 0.0, 0.0, 0.0 );
}

LineSegment::LineSegment( const LineSegment& lineSegment )
{
	vertex[0] = lineSegment.vertex[0];
	vertex[1] = lineSegment.vertex[1];
}

LineSegment::LineSegment( const Vector& vertex0, const Vector& vertex1 )
{
	vertex[0] = vertex0;
	vertex[1] = vertex1;
}

LineSegment::~LineSegment( void )
{
}

double LineSegment::Length( void )
{
	Vector vector;
	vector.Subtract( vertex[1], vertex[0] );
	return vector.Length();
}

void LineSegment::Lerp( double lambda, Vector& point ) const
{
	point.AddScale( vertex[0], lambda, vertex[1], 1.0 - lambda );
}

double LineSegment::LerpInverse( const Vector& point ) const
{
	return 0.0;
}

// LineSegment.cpp