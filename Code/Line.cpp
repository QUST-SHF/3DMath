// Line.cpp

#include "Line.h"
#include "LineSegment.h"
#include "Function.h"
#include "AffineTransform.h"

using namespace _3DMath;

Line::Line( void )
{
}

Line::Line( const Vector& center, const Vector& normal )
{
	this->center = center;
	normal.GetNormalized( this->normal );
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

void Line::Reflect( Vector& vector ) const
{
	normal.Reflect( vector );
}

void Line::GetIdealCenter( Vector& point ) const
{
	double lambda = -center.Dot( normal );
	point.AddScale( center, normal, lambda );
}

void Line::Lerp( double lambda, Vector& point ) const
{
	point.AddScale( center, normal, lambda );
}

void Line::Transform( const AffineTransform& affineTransform, const LinearTransform* normalTransform /*= nullptr*/ )
{
	affineTransform.Transform( center );

	static LinearTransform normalTransformStorage;
	if( !normalTransform )
	{
		affineTransform.linearTransform.GetNormalTransform( normalTransformStorage );
		normalTransform = &normalTransformStorage;
	}

	normalTransform->Transform( normal );
}

// Line.cpp