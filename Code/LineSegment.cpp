// LineSegment.cpp

#include "LineSegment.h"
#include "AxisAlignedBox.h"

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

bool LineSegment::LerpInverse( double& lambda, const Vector& point, double eps /*= EPSILON*/ ) const
{
	Vector vecX, vecY;
	vecX.Subtract( point, vertex[0] );
	vecY.Subtract( vertex[1], vertex[0] );

	Vector cross;
	cross.Cross( vecX, vecY );
	if( cross.Length() > eps )
		return false;

	lambda = vecX.Dot( vecY ) / vecY.Dot( vecY );
	return true;
}

bool LineSegment::ContainsPoint( const Vector& point, double eps /*= EPSILON*/ ) const
{
	double lambda;
	if( !LerpInverse( lambda, point, eps ) )
		return false;

	return AxisAlignedBox::InInterval( 0.0, 1.0, lambda, eps );
}

// LineSegment.cpp