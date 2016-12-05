// AxisAlignedBox.h

#pragma once

#include "Defines.h"
#include "Vector.h"

namespace _3DMath
{
	class AxisAlignedBox;
	class Triangle;
	class LineSegment;
}

class _3DMATH_API _3DMath::AxisAlignedBox
{
public:

	AxisAlignedBox( void );
	AxisAlignedBox( const AxisAlignedBox& box );
	AxisAlignedBox( const Vector& negCorner, const Vector& posCorner );
	~AxisAlignedBox( void );

	void GrowToIncludePoint( const Vector& point );

	bool Intersect( const AxisAlignedBox& boxA, const AxisAlignedBox& boxB );
	void Combine( const AxisAlignedBox& boxA, const AxisAlignedBox& boxB );

	void GetCenter( Vector& center ) const;
	void SplitInTwo( AxisAlignedBox& boxA, AxisAlignedBox& boxB ) const;

	bool ContainsPoint( const Vector& point ) const;
	bool ContainsTriangle( const Triangle& triangle ) const;
	bool ContainsLineSegment( const LineSegment& lineSegment ) const;

	static void ExpandInterval( double& min, double& max, double value );
	static bool InInterval( double min, double max, double value );

	Vector negCorner, posCorner;
};

// AxisAlignedBox.h