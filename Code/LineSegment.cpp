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
	point.AddScale( vertex[0], 1.0 - lambda, vertex[1], lambda );
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

bool LineSegment::IntersectsWith( const LineSegment& lineSegment, Vector* intersectionPoint /*= nullptr*/, double eps /*= EPSILON*/ ) const
{
	const Vector& a = vertex[0];
	const Vector& b = vertex[1];
	const Vector& c = lineSegment.vertex[0];
	const Vector& d = lineSegment.vertex[1];

	Vector q = c - a;
	Vector r = d - c;
	Vector s = b - a;

	Vector q_cross_s, q_cross_r, s_cross_r;
	q_cross_s.Cross( q, s );
	q_cross_r.Cross( q, r );
	s_cross_r.Cross( s, r );

	double r_dot_r = r.Dot( r );
	double s_dot_s = s.Dot( s );
	double q_dot_r = q.Dot( r );
	double q_dot_s = q.Dot( s );
	double r_dot_s = r.Dot( s );

	Vector v;

	v = q_cross_s * -r_dot_r + q_cross_r * r_dot_s + s_cross_r * -q_dot_r;
	if( v.Dot( v ) >= eps )
		return false;

	v = q_cross_s * -r_dot_s + q_cross_r * s_dot_s + s_cross_r * -q_dot_s;
	if( v.Dot( v ) >= eps )
		return false;

	double scalar = 1.0 / -s_cross_r.Dot( s_cross_r );

	double t0 = ( r_dot_s * q_dot_r - r_dot_r * q_dot_s ) * scalar;
	double t1 = ( s_dot_s * q_dot_r - r_dot_s * q_dot_s ) * scalar;

	if( t0 < 0.0 || t0 > 1.0 || t1 < 0.0 || t1 > 1.0 )
		return false;

	if( intersectionPoint )
		intersectionPoint->Lerp( a, b, t0 );

	return true;
}

// LineSegment.cpp