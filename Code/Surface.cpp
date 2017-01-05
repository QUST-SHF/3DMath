// Surface.cpp

#include "Surface.h"
#include "LineSegment.h"
#include "Line.h"
#include "AffineTransform.h"
#include "Renderer.h"
#include "Circle.h"

using namespace _3DMath;

//-------------------------------------------------------------------------------
//                                  SurfacePoint
//-------------------------------------------------------------------------------

SurfacePoint::SurfacePoint( int surfaceHandle )
{
	this->surfaceHandle = surfaceHandle;
}

/*virtual*/ SurfacePoint::~SurfacePoint( void )
{
}

//-------------------------------------------------------------------------------
//                               GenericSurfacePoint
//-------------------------------------------------------------------------------

GenericSurfacePoint::GenericSurfacePoint( int surfaceHandle ) : SurfacePoint( surfaceHandle )
{
}

/*virtual*/ GenericSurfacePoint::~GenericSurfacePoint( void )
{
}

/*virtual*/ bool GenericSurfacePoint::GetLocation( Vector& location ) const
{
	location = transform.translation;
	return true;
}

/*virtual*/ bool GenericSurfacePoint::GetTangentSpace( LinearTransform& tangentSpace ) const
{
	tangentSpace = transform.linearTransform;
	return true;
}

//-------------------------------------------------------------------------------
//                                    Surface
//-------------------------------------------------------------------------------

Surface::Surface( void )
{
}

/*virtual*/ Surface::~Surface( void )
{
}

/*virtual*/ bool Surface::Render( Renderer& renderer, const Vector& color, double alpha, const AffineTransform* transform /*= nullptr*/ ) const
{
	return false;
}

/*virtual*/ bool Surface::FindDirectPath( const SurfacePoint* surfacePointA, const SurfacePoint* surfacePointB, VectorArray& pointArray, double maxDistance, const Plane* plane /*= nullptr*/ ) const
{
	bool success = false;
	SurfacePoint* surfacePoint = nullptr;

	do
	{
		if( surfacePointA->surfaceHandle != GetHandle() || surfacePointB->surfaceHandle != GetHandle() )
			break;

		LineSegment lineSegment;

		surfacePointA->GetLocation( lineSegment.vertex[0] );
		surfacePointB->GetLocation( lineSegment.vertex[1] );

		Vector midPoint;
		lineSegment.Lerp( 0.5, midPoint );

		SurfacePoint* surfacePoint = nullptr;
		if( plane )
			surfacePoint = GetNearestSurfacePointInPlane( midPoint, *plane );
		else
			surfacePoint = GetNearestSurfacePoint( midPoint );
		if( !surfacePoint )
			break;

		Vector location;
		surfacePoint->GetLocation( location );

		if( location.Distance( midPoint ) <= maxDistance )
		{
			pointArray.push_back( lineSegment.vertex[0] );
			pointArray.push_back( lineSegment.vertex[1] );
		}
		else
		{
			VectorArray pointArrayA;
			if( !FindDirectPath( surfacePointA, surfacePoint, pointArrayA, maxDistance ) )
				break;

			VectorArray pointArrayB;
			if( !FindDirectPath( surfacePoint, surfacePointB, pointArrayB, maxDistance ) )
				break;

			for( int i = 0; i < ( signed )pointArrayA.size() - 1; i++ )
				pointArray.push_back( pointArrayA[i] );

			for( int i = 0; i < ( signed )pointArrayB.size(); i++ )
				pointArray.push_back( pointArrayB[i] );
		}

		success = true;
	}
	while( false );

	delete surfacePoint;

	return success;
}

/*virtual*/ SurfacePoint* Surface::GetNearestSurfacePointInPlane( const Vector& point, const Plane& plane ) const
{
	return nullptr;
}

//-------------------------------------------------------------------------------
//                            PlaneSurface::Point
//-------------------------------------------------------------------------------

PlaneSurface::Point::Point( int surfaceHandle ) : SurfacePoint( surfaceHandle )
{
}

/*virtual*/ PlaneSurface::Point::~Point( void )
{
}

/*virtual*/ bool PlaneSurface::Point::GetLocation( Vector& location ) const
{
	location = this->location;
	return true;
}

/*virtual*/ bool PlaneSurface::Point::GetTangentSpace( LinearTransform& tangentSpace ) const
{
	PlaneSurface* surface = ( PlaneSurface* )HandleObject::Dereference( surfaceHandle );
	if( !surface )
		return false;

	Vector center;
	surface->plane.GetCenter( center );

	tangentSpace.zAxis = surface->plane.normal;
	tangentSpace.xAxis.Subtract( center, location );
	tangentSpace.xAxis.Normalize();		// TODO: Oops, what if this doesn't normalize?
	tangentSpace.yAxis.Cross( tangentSpace.zAxis, tangentSpace.xAxis );

	return true;
}

//-------------------------------------------------------------------------------
//                                  PlaneSurface
//-------------------------------------------------------------------------------

PlaneSurface::PlaneSurface( const Plane& plane )
{
	this->plane = plane;
}

/*virtual*/ PlaneSurface::~PlaneSurface( void )
{
}

/*virtual*/ bool PlaneSurface::Render( Renderer& renderer, const Vector& color, double alpha, const AffineTransform* transform /*= nullptr*/ ) const
{
	double radius = 10.0;
	renderer.DrawPlane( plane, radius, color, alpha, transform );
	return true;
}

/*virtual*/ Surface::Side PlaneSurface::GetSide( const Vector& point, double eps /*= EPSILON*/ ) const
{
	Plane::Side planeSide = plane.GetSide( point, eps );
	if( planeSide == Plane::SIDE_FRONT )
		return OUTSIDE;
	if( planeSide == Plane::SIDE_BACK )
		return INSIDE;
	return NEITHER_SIDE;
}

/*virtual*/ SurfacePoint* PlaneSurface::GetNearestSurfacePoint( const Vector& point ) const
{
	Vector nearestPoint = point;
	plane.NearestPoint( nearestPoint );

	Point* surfacePoint = new Point( GetHandle() );
	surfacePoint->location = nearestPoint;
	return surfacePoint;
}

/*virtual*/ SurfacePoint* PlaneSurface::FindSingleIntersection( const LineSegment& lineSegment ) const
{
	Vector intersectionPoint;
	if( !plane.Intersect( lineSegment, intersectionPoint ) )
		return nullptr;

	Point* surfacePoint = new Point( GetHandle() );
	surfacePoint->location = intersectionPoint;
	return surfacePoint;
}

/*virtual*/ bool PlaneSurface::FindDirectPath( const SurfacePoint* surfacePointA, const SurfacePoint* surfacePointB, VectorArray& pointArray, double maxDistance, const Plane* plane /*= nullptr*/ ) const
{
	if( surfacePointA->surfaceHandle != GetHandle() || surfacePointB->surfaceHandle != GetHandle() )
		return false;

	Vector point;

	surfacePointA->GetLocation( point );
	pointArray.push_back( point );

	surfacePointB->GetLocation( point );
	pointArray.push_back( point );

	return true;
}

//-------------------------------------------------------------------------------
//                              SphereSurface::Point
//-------------------------------------------------------------------------------

SphereSurface::Point::Point( int surfaceHandle ) : SurfacePoint( surfaceHandle )
{
}

/*virtual*/ SphereSurface::Point::~Point( void )
{
}

/*virtual*/ bool SphereSurface::Point::GetLocation( Vector& location ) const
{
	location = this->location;
	return true;
}

/*virtual*/ bool SphereSurface::Point::GetTangentSpace( LinearTransform& tangentSpace ) const
{
	// TODO: Write this.
	return false;
}

//-------------------------------------------------------------------------------
//                                SphereSurface
//-------------------------------------------------------------------------------

SphereSurface::SphereSurface( const Sphere& sphere )
{
	this->sphere = sphere;
}

/*virtual*/ SphereSurface::~SphereSurface( void )
{
}

/*virtual*/ bool SphereSurface::Render( Renderer& renderer, const Vector& color, double alpha, const AffineTransform* transform /*= nullptr*/ ) const
{
	renderer.DrawSphere( sphere, color, alpha, transform );
	return true;
}

/*virtual*/ Surface::Side SphereSurface::GetSide( const Vector& point, double eps /*= EPSILON*/ ) const
{
	double distance = sphere.Distance( point );
	if( distance > eps )
		return OUTSIDE;
	if( distance < -eps )
		return INSIDE;
	return NEITHER_SIDE;
}

/*virtual*/ SurfacePoint* SphereSurface::GetNearestSurfacePoint( const Vector& point ) const
{
	Vector unitVector;
	unitVector.Subtract( sphere.center, point );
	unitVector.Normalize();

	VectorArray intersectionPoints;
	sphere.RayCast( point, unitVector, intersectionPoints );
	if( intersectionPoints.size() == 0 )
		return nullptr;

	int i = 0;
	if( intersectionPoints.size() > 1 && point.Distance( intersectionPoints[1] ) < point.Distance( intersectionPoints[0] ) )
		i = 1;

	Point* surfacePoint = new Point( GetHandle() );
	surfacePoint->location = intersectionPoints[i];
	return surfacePoint;
}

/*virtual*/ SurfacePoint* SphereSurface::FindSingleIntersection( const LineSegment& lineSegment ) const
{
	VectorArray intersectionPoints;
	sphere.Intersect( lineSegment, intersectionPoints );
	if( intersectionPoints.size() != 1 )
		return nullptr;

	Point* surfacePoint = new Point( GetHandle() );
	surfacePoint->location = intersectionPoints[0];
	return surfacePoint;
}

/*virtual*/ bool SphereSurface::FindDirectPath( const SurfacePoint* surfacePointA, const SurfacePoint* surfacePointB, VectorArray& pointArray, double maxDistance, const Plane* plane /*= nullptr*/ ) const
{
	Vector pointA, pointB;
	surfacePointA->GetLocation( pointA );
	surfacePointB->GetLocation( pointB );

	Plane defaultPlane;
	if( !plane )
	{
		plane = &defaultPlane;
		defaultPlane.SetUsingTriangle( Triangle( pointA, sphere.center, pointB ) );
	}

	Circle circle;
	if( !circle.SetAsIntersectionOf( *plane, sphere ) )
		return false;

	LinearTransform linearTransform;
	linearTransform.xAxis = pointA - circle.center;
	linearTransform.yAxis = pointB - circle.center;
	linearTransform.zAxis = circle.normal;

	double det = linearTransform.Determinant();
	if( fabs( det ) < EPSILON )
		return false;

	if( det < 0.0 )
		circle.normal.Negate();

	double angle = linearTransform.xAxis.AngleBetween( linearTransform.yAxis );
	double arcLength = angle * circle.radius;
	double rotationCount = ceil( arcLength / maxDistance );
	double angleDelta = angle / rotationCount;

	Line line( circle.center, circle.normal );

	AffineTransform rotationTransform;
	rotationTransform.SetRotation( line, angleDelta );

	pointArray.push_back( pointA );

	for( int i = 0; i < ( int )rotationCount - 1; i++ )
	{
		rotationTransform.Transform( pointA );
		pointArray.push_back( pointA );
	}

	pointArray.push_back( pointB );

	return true;
}

// Surface.cpp