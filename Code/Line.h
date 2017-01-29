// Line.h

#pragma once

#include "Defines.h"
#include "Vector.h"

namespace _3DMath
{
	class Line;
	class LineSegment;
	class Vector;
	class AffineTransform;
	class LinearTransform;
}

class _3DMATH_API _3DMath::Line
{
public:

	Line( void );
	Line( const Vector& center, const Vector& normal );
	Line( const LineSegment& lineSegment );
	~Line( void );

	double ShortestDistance( const Vector& point ) const;
	void Reflect( Vector& vector ) const;
	void GetIdealCenter( Vector& point ) const;
	void Lerp( double lambda, Vector& point ) const;
	void Transform( const AffineTransform& affineTransform, const LinearTransform* normalTransform = nullptr );

	Vector center, normal;
};

// Line.h
