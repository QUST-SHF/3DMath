// Line.h

#pragma once

#include "Defines.h"
#include "Vector.h"

namespace _3DMath
{
	class Line;
	class LineSegment;
	class Vector;
}

class _3DMATH_API _3DMath::Line
{
public:

	Line( void );
	Line( const Vector& center, const Vector& normal );
	Line( const LineSegment& lineSegment );
	~Line( void );

	double ShortestDistance( const Vector& point ) const;

	Vector center, normal;
};

// Line.h
