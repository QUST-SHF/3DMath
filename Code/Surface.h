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
	class Renderer;
}

// Surfaces abstract away the idea of a point upon them, because, depending
// on the needs of the surface, more or less information than a single point in
// space may need to be stored with point representation.  For example, a
// parametrically represented surface may represent a point as nothing more than
// the parameters that generate the surface point.  This has other advantages as well since
// it becomes easier to get other information about the surface at the surface-
// point, such as the tangent space and maybe some calculus stuff, like curviture or curl.
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
		BOTH_SIDES,		// Some surfaces, like the Klein bottle, have no inside or outside.
	};

	virtual bool Render( Renderer& renderer, const Vector& color, double alpha, const AffineTransform* transform = nullptr ) const;
	virtual Side GetSide( const Vector& point, double eps = EPSILON ) const = 0;
	virtual SurfacePoint* GetNearestSurfacePoint( const Vector& point ) const = 0;
	virtual SurfacePoint* GetNearestSurfacePointInPlane( const Vector& point, const Plane& plane ) const;
	virtual SurfacePoint* FindSingleIntersection( const LineSegment& lineSegment ) const = 0;
	virtual bool FindDirectPath( const SurfacePoint* surfacePointA, const SurfacePoint* surfacePointB, VectorArray& pointArray, double maxDistance, const Plane* plane = nullptr ) const;
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

	virtual bool Render( Renderer& renderer, const Vector& color, double alpha, const AffineTransform* transform = nullptr ) const override;
	virtual Side GetSide( const Vector& point, double eps = EPSILON ) const override;
	virtual SurfacePoint* GetNearestSurfacePoint( const Vector& point ) const override;
	virtual SurfacePoint* FindSingleIntersection( const LineSegment& lineSegment ) const override;
	virtual bool FindDirectPath( const SurfacePoint* surfacePointA, const SurfacePoint* surfacePointB, VectorArray& pointArray, double maxDistance, const Plane* plane = nullptr ) const override;

	Plane plane;
};

class _3DMATH_API _3DMath::SphereSurface : public _3DMath::Surface
{
public:

	SphereSurface( const Sphere& sphere );
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

	virtual bool Render( Renderer& renderer, const Vector& color, double alpha, const AffineTransform* transform = nullptr ) const override;
	virtual Side GetSide( const Vector& point, double eps = EPSILON ) const override;
	virtual SurfacePoint* GetNearestSurfacePoint( const Vector& point ) const override;
	virtual SurfacePoint* FindSingleIntersection( const LineSegment& lineSegment ) const override;
	virtual bool FindDirectPath( const SurfacePoint* surfacePointA, const SurfacePoint* surfacePointB, VectorArray& pointArray, double maxDistance, const Plane* plane = nullptr ) const override;

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

// TODO: We might support polynomial surfaces too.  These are zero sets of polynomials.
//class _3DMATH_API _3DMath::PolynomialSurface : public _3DMath::Surface
//{
//public:
//};

// Surface.h
