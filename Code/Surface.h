// Surface.h

#pragma once

#include "Defines.h"
#include "Vector.h"
#include "AffineTransform.h"
#include "HandleObject.h"
#include "Plane.h"
#include "Sphere.h"

namespace _3DMath
{
	class Surface;
	class SurfacePoint;
	class GenericSurfacePoint;
	class PlaneSurface;
	class SphereSurface;
	class LinearTransform;
	class LineSegment;
	class AffineTransform;
}

class _3DMATH_API _3DMath::SurfacePoint
{
public:

	SurfacePoint( int surfaceHandle );
	virtual ~SurfacePoint( void );

	virtual bool GetLocation( Vector& location ) const = 0;
	virtual bool GetTangentSpace( LinearTransform& tangentSpace ) const = 0;

	int surfaceHandle;
};

class _3DMATH_API _3DMath::GenericSurfacePoint : public SurfacePoint
{
public:

	GenericSurfacePoint( int surfaceHandle );
	virtual ~GenericSurfacePoint( void );

	virtual bool GetLocation( Vector& location ) const override;
	virtual bool GetTangentSpace( LinearTransform& tangentSpace ) const override;

	AffineTransform transform;
};

// This provides an abstraction for any kind of surface.
// It doesn't matter how it's represented; we want it to provide
// the following interface.  It may be parametric, explicit, implicit,
// and so on, but in all cases, we can think of it as any kind of
// surface upon which there are tangent spaces and perhaps a concept
// of differentiation upon it too.  Algorithms that are designed to
// work with surfaces will then, it is hoped, work with any kind of
// surface we derive from this base class.
class _3DMATH_API _3DMath::Surface : public _3DMath::HandleObject
{
public:

	Surface( void );
	virtual ~Surface( void );

	enum Side
	{
		INSIDE,
		OUTSIDE,
		NEITHER_SIDE,
	};

	virtual Side GetSide( const Vector& point, double eps = EPSILON ) const = 0;
	virtual SurfacePoint* GetNearestSurfacePoint( const Vector& point ) const = 0;
	virtual SurfacePoint* FindIntersection( const LineSegment& lineSegment ) const = 0;
	virtual bool FindDirectPath( const SurfacePoint* surfacePointA, const SurfacePoint* surfacePointB, VectorArray& pointArray, double maxDistanceFromSurface ) const;
};

class _3DMATH_API _3DMath::PlaneSurface : public _3DMath::Surface
{
public:

	PlaneSurface( const Plane& plane );
	virtual ~PlaneSurface( void );

	class _3DMATH_API Point : public SurfacePoint
	{
	public:

		Point( int surfaceHandle );
		virtual ~Point( void );

		virtual bool GetLocation( Vector& location ) const override;
		virtual bool GetTangentSpace( LinearTransform& tangentSpace ) const override;

		Vector location;
	};

	virtual Side GetSide( const Vector& point, double eps = EPSILON ) const override;
	virtual SurfacePoint* GetNearestSurfacePoint( const Vector& point ) const override;
	virtual SurfacePoint* FindIntersection( const LineSegment& lineSegment ) const override;
	virtual bool FindDirectPath( const SurfacePoint* surfacePointA, const SurfacePoint* surfacePointB, VectorArray& pointArray, double maxDistanceFromSurface ) const override;

	Plane plane;
};

class _3DMATH_API _3DMath::SphereSurface : public _3DMath::Surface
{
public:

	SphereSurface( void );
	virtual ~SphereSurface( void );

	class _3DMATH_API Point : public SurfacePoint
	{
	public:

		Point( int surfaceHandle );
		virtual ~Point( void );

		virtual bool GetLocation( Vector& location ) const override;
		virtual bool GetTangentSpace( LinearTransform& tangentSpace ) const override;

		Vector location;
	};

	virtual Side GetSide( const Vector& point, double eps = EPSILON ) const override;
	virtual SurfacePoint* GetNearestSurfacePoint( const Vector& point ) const override;
	virtual SurfacePoint* FindIntersection( const LineSegment& lineSegment ) const override;

	Sphere sphere;
};

// TODO: We might one day want to support something like a NURBS surface or a B-spline surface.
//class _3DMATH_API _3DMath::NurbsSurface : public _3DMath::Surface
//{
//public:
//};

// TODO: Might we ever want to think of a triangle mesh as a surface?
//class _3DMATH_API _3DMath::TriangleMeshSurface : public _3DMath::Surface
//{
//public:
//};

// Surface.h
