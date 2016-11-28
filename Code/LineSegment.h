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

	Vector vertex[2];
};

// LineSegment.h