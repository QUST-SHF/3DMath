// AxisAlignedBox.h

#pragma once

#include "Defines.h"
#include "Vector.h"

namespace _3DMath
{
	class AxisAlignedBox;
	class Triangle;
	class LineSegment;
	class Plane;
	class Renderer;
}

class _3DMATH_API _3DMath::AxisAlignedBox
{
public:

	AxisAlignedBox( void );
	AxisAlignedBox( const AxisAlignedBox& box );
	AxisAlignedBox( const Vector& negCorner, const Vector& posCorner );
	~AxisAlignedBox( void );

	void GrowToIncludePoint( const Vector& point );
	void SetCenterAndDimensions( const Vector& center, const Vector& dimensions );

	bool Intersect( const AxisAlignedBox& boxA, const AxisAlignedBox& boxB );
	void Combine( const AxisAlignedBox& boxA, const AxisAlignedBox& boxB );

	void GetCenter( Vector& center ) const;
	void SplitInTwo( AxisAlignedBox& boxA, AxisAlignedBox& boxB, Plane* plane = nullptr, int split = -1 ) const;

	bool ContainsPoint( const Vector& point, double eps = EPSILON ) const;
	bool ContainsTriangle( const Triangle& triangle, double eps = EPSILON ) const;
	bool ContainsLineSegment( const LineSegment& lineSegment, double eps = EPSILON ) const;
	bool IntersectsWithLineSegment( const LineSegment& lineSegment, double eps = EPSILON ) const;

	static void ExpandInterval( double& min, double& max, double value );
	static bool InInterval( double min, double max, double value, double eps = EPSILON );

	void Render( Renderer& renderer ) const;

	Vector negCorner, posCorner;
};

// AxisAlignedBox.h