// Plane.h

#pragma once

#include "Defines.h"
#include "Vector.h"
#include "Triangle.h"

namespace _3DMath
{
	class Plane;
	class LineSegment;
	class Line;
	class AffineTransform;
	class LinearTransform;
}

class _3DMATH_API _3DMath::Plane
{
public:

	Plane( void );
	Plane( const Vector& center, const Vector& normal );
	~Plane( void );

	void SetUsingTriangle( const Triangle& triangle );
	void SetCenterAndNormal( const Vector& center, const Vector& normal );
	void GetCenter( Vector& center ) const;

	enum Side
	{
		SIDE_FRONT,
		SIDE_BACK,
		SIDE_NEITHER,
	};

	Side GetSide( const Vector& point, double eps = EPSILON ) const;
	double Distance( const Vector& point ) const;
	void NearestPoint( Vector& point ) const;
	bool Intersect( const LineSegment& lineSegment, Vector& intersectionPoint, double eps = EPSILON ) const;
	bool Intersect( const Line& line, Vector& intersectionPoint, double eps = EPSILON ) const;
	bool SplitTriangle( const Triangle& triangle, TriangleList& frontList, TriangleList& backList ) const;
	void Reflect( Vector& point ) const;
	void Transform( const AffineTransform& affineTransform, const LinearTransform* normalTransform = nullptr );
	bool IsEqualTo( const Plane& plane, double eps = EPSILON ) const;
	bool FitToPoints( const VectorList& vectorList );

	Vector normal;
	double centerDotNormal;
};

namespace _3DMath
{
	typedef std::list< Plane > PlaneList;
}

// Plane.h