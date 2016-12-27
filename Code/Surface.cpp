// Surface.cpp

#include "Surface.h"
#include "LineSegment.h"

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

/*virtual*/ bool Surface::FindDirectPath( const SurfacePoint* surfacePointA, const SurfacePoint* surfacePointB, VectorArray& pointArray, double maxDistanceFromSurface ) const
{
	bool success = false;
	SurfacePoint* surfacePoint = nullptr;

	do
	{
		LineSegment lineSegment;

		surfacePointA->GetLocation( lineSegment.vertex[0] );
		surfacePointB->GetLocation( lineSegment.vertex[1] );

		Vector midPoint;
		lineSegment.Lerp( 0.5, midPoint );

		SurfacePoint* surfacePoint = GetNearestSurfacePoint( midPoint );
		if( !surfacePoint )
			break;

		Vector location;
		surfacePoint->GetLocation( location );

		if( location.Distance( midPoint ) <= maxDistanceFromSurface )
		{
			pointArray.push_back( lineSegment.vertex[0] );
			pointArray.push_back( lineSegment.vertex[1] );
		}
		else
		{
			VectorArray pointArrayA;
			if( !FindDirectPath( surfacePointA, surfacePoint, pointArrayA, maxDistanceFromSurface ) )
				break;

			VectorArray pointArrayB;
			if( !FindDirectPath( surfacePoint, surfacePointB, pointArrayB, maxDistanceFromSurface ) )
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

/*virtual*/ SurfacePoint* PlaneSurface::FindIntersection( const LineSegment& lineSegment ) const
{
	Vector intersectionPoint;
	plane.Intersect( lineSegment, intersectionPoint );

	Point* surfacePoint = new Point( GetHandle() );
	surfacePoint->location = intersectionPoint;
	return surfacePoint;
}

/*virtual*/ bool PlaneSurface::FindDirectPath( const SurfacePoint* surfacePointA, const SurfacePoint* surfacePointB, VectorArray& pointArray, double maxDistanceFromSurface ) const
{
	if( surfacePointA->surfaceHandle != GetHandle() || surfacePointB->surfaceHandle != GetHandle() )
		return false;

	pointArray.clear();

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

SphereSurface::SphereSurface( void )
{
}

/*virtual*/ SphereSurface::~SphereSurface( void )
{
}

/*virtual*/ Surface::Side SphereSurface::GetSide( const Vector& point, double eps /*= EPSILON*/ ) const
{
	double distance = sphere.Distance( point );
	if( distance > eps )
		return OUTSIDE;
	if( distance < eps )
		return INSIDE;
	return NEITHER_SIDE;
}

/*virtual*/ SurfacePoint* SphereSurface::GetNearestSurfacePoint( const Vector& point ) const
{
	Vector unitVector;
	unitVector.Subtract( sphere.center, point );
	unitVector.Normalize();

	Vector intersectionPoints[2];
	int count = sphere.RayCast( point, unitVector, intersectionPoints );
	if( count == 0 )
		return nullptr;

	int i = 0;
	if( count > 1 && point.Distance( intersectionPoints[1] ) < point.Distance( intersectionPoints[0] ) )
		i = 1;

	Point* surfacePoint = new Point( GetHandle() );
	surfacePoint->location = intersectionPoints[i];
	return surfacePoint;
}

/*virtual*/ SurfacePoint* SphereSurface::FindIntersection( const LineSegment& lineSegment ) const
{
	Vector intersectionPoints[2];
	int count = sphere.Intersect( lineSegment, intersectionPoints );
	if( count == 0 || count == 2 )
		return nullptr;

	Point* surfacePoint = new Point( GetHandle() );
	surfacePoint->location = intersectionPoints[0];
	return surfacePoint;
}

// Surface.cpp