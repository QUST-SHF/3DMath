// LineSegment.h

#pragma once

#include "Defines.h"
#include "Vector.h"

namespace _3DMath
{
	class LineSegment;
}

class _3DMATH_API _3DMath::LineSegment
{
public:

	LineSegment( void );
	LineSegment( const LineSegment& lineSegment );
	LineSegment( const Vector& vertex0, const Vector& vertex1 );
	~LineSegment( void );

	double Length( void );
	void Lerp( double lambda, Vector& point ) const;
	double LerpInverse( const Vector& point ) const;

	Vector vertex[2];
};

// LineSegment.h