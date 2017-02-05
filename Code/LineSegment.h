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
	bool LerpInverse( double& lambda, const Vector& point, double eps = EPSILON ) const;
	bool ContainsPoint( const Vector& point, double eps = EPSILON ) const;
	bool IntersectsWith( const LineSegment& lineSegment, Vector* intersectionPoint = nullptr, double eps = EPSILON ) const;

	Vector vertex[2];
};

namespace _3DMath
{
	typedef std::list< LineSegment > LineSegmentList;
}

// LineSegment.h