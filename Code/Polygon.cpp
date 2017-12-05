// Polygon.cpp

#include "Polygon.h"
#include "Triangle.h"
#include "IndexTriangle.h"
#include "LineSegment.h"
#include "Surface.h"
#include "AffineTransform.h"
#include "Graph.h"
#include "Exception.h"
#include "ListFunctions.h"

using namespace _3DMath;

Polygon::Polygon( void )
{
	vertexArray = new VectorArray();
	indexTriangleList = new IndexTriangleList();
}

/*virtual*/ Polygon::~Polygon( void )
{
	delete vertexArray;
	delete indexTriangleList;
}

void Polygon::Transform( const AffineTransform& transform )
{
	transform.Transform( *vertexArray );
}

bool Polygon::GetPlane( Plane& plane ) const
{
	if( vertexArray->size() < 3 )
		return false;

	Vector center;
	GetCenter( center );

	Vector normal;
	normal.Set( 0.0, 0.0, 0.0 );
	
	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
	{
		// This is the Newel method.
		int j = ( i + 1 ) % vertexArray->size();
		const Vector& pointA = ( *vertexArray )[i];
		const Vector& pointB = ( *vertexArray )[j];
		normal.x += ( pointA.y - pointB.y ) * ( pointA.z + pointB.z );
		normal.y += ( pointA.z - pointB.z ) * ( pointA.x + pointB.x );
		normal.z += ( pointA.x - pointB.x ) * ( pointA.y + pointB.y );
	}

	plane.SetCenterAndNormal( center, normal );
	return true;
}

void Polygon::GetCenter( Vector& center ) const
{
	center.Set( 0.0, 0.0, 0.0 );
	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
		center.Add( ( *vertexArray )[i] );
	center.Scale( 1.0 / double( vertexArray->size() ) );
}

bool Polygon::GetTriangleAverageCenter( Vector& center ) const
{
	if( indexTriangleList->size() == 0 )
		return false;

	center.Set( 0.0, 0.0, 0.0 );

	for( IndexTriangleList::const_iterator iter = indexTriangleList->cbegin(); iter != indexTriangleList->cend(); iter++ )
	{
		const IndexTriangle& indexTriangle = *iter;

		Triangle triangle;
		indexTriangle.GetTriangle( triangle, vertexArray );

		Vector triangleCenter;
		triangle.GetCenter( triangleCenter );

		center.Add( triangleCenter );
	}

	center.Scale( 1.0 / double( indexTriangleList->size() ) );
	return true;
}

// This assumes that we're properly tessellated.
bool Polygon::ContainsPoint( const Vector& point, double eps /*= EPSILON*/ ) const
{
	for( IndexTriangleList::const_iterator iter = indexTriangleList->cbegin(); iter != indexTriangleList->cend(); iter++ )
	{
		const IndexTriangle& indexTriangle = *iter;

		Triangle triangle;
		indexTriangle.GetTriangle( triangle, vertexArray );

		if( triangle.ContainsPoint( point, eps ) )
			return true;
	}

	return false;
}

// Add extraneous points to the polygon until no edge is greater than the given minimum distance.
// Of course, since the geometry of the polygon doesn't change, the visible edge-lengths remain the same.
// Note that this may invalidate a current tessellation of the polygon.
void Polygon::IncreaseDensity( double minDistance )
{
	int i;

	do
	{	
		for( i = 0; i < ( signed )vertexArray->size(); i++ )
		{
			int j = ( i + 1 ) % vertexArray->size();
			LineSegment lineSegment;
			lineSegment.vertex[0] = ( *vertexArray )[i];
			lineSegment.vertex[1] = ( *vertexArray )[j];
			if( lineSegment.Length() > minDistance )
			{
				Vector midPoint;
				lineSegment.Lerp( 0.5, midPoint );
				vertexArray->insert( vertexArray->begin() + j, midPoint );
				break;
			}
		}	
	}
	while( i < ( signed )vertexArray->size() );
}

// Here we remove any extraneous vertices.  These are those that
// are co-linear with respect to their immediate adjacencies.
// Note that this may invalidate a current tessellation of the polygon.
void Polygon::MinimizeDensity( void )
{
	while( true )
	{
		int i;
		for( i = 0; i < ( signed )vertexArray->size(); i++ )
		{
			Triangle triangle;
			for( int j = 0; j < 3; j++ )
				triangle.vertex[j] = ( *vertexArray )[ ( i + j ) % vertexArray->size() ];

			if( triangle.Area() < EPSILON )
				break;
		}

		if( i == ( signed )vertexArray->size() )
			break;

		vertexArray->erase( vertexArray->begin() + ( i + 1 ) % vertexArray->size() );
	}
}

void Polygon::GetCopy( Polygon& polygon ) const
{
	polygon.vertexArray->clear();
	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
		polygon.vertexArray->push_back( ( *vertexArray )[i] );

	polygon.indexTriangleList->clear();
}

bool Polygon::SplitAgainstSurface( const Surface* surface, PolygonList& polygonList, double minDistance, double maxDistance, double eps /*= EPSILON*/ ) const
{
	Polygon* polygon = new Polygon();
	GetCopy( *polygon );

	PolygonList polygonQueue;
	polygonQueue.push_back( polygon );

	while( polygonQueue.size() > 0 )
	{
		PolygonList::iterator iter = polygonQueue.begin();
		polygon = *iter;
		polygonQueue.erase( iter );

		Polygon* polygonA = nullptr;
		Polygon* polygonB = nullptr;

		if( !polygon->SplitInTwoIfNeeded( polygonA, polygonB, eps ) &&
			!polygon->SplitInTwoAgainstSurface( surface, polygonA, polygonB, minDistance, maxDistance, eps ) )
		{
			polygonList.push_back( polygon );
		}
		else
		{
			delete polygon;
			polygonQueue.push_back( polygonA );
			polygonQueue.push_back( polygonB );
		}
	}

	if( polygonList.size() > 1 )
		return true;

	FreeList< Polygon >( polygonList );
	return false;
}

// This removes a special case of self-intersection (self-tangential) that can
// mess up the correctness of the tessellation algorithm and possibly others.
bool Polygon::SplitInTwoIfNeeded( Polygon*& polygonA, Polygon*& polygonB, double eps /*= EPSILON*/ )
{
	polygonA = nullptr;
	polygonB = nullptr;

	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
	{
		const Vector& pointA = ( *vertexArray )[i];

		for( int j = i + 1; j < ( signed )vertexArray->size(); j++ )
		{
			const Vector& pointB = ( *vertexArray )[j];

			if( pointA.IsEqualTo( pointB, eps ) )
			{
				polygonA = new Polygon();
				polygonB = new Polygon();

				int k = i;
				while( k != j )
				{
					polygonA->vertexArray->push_back( ( *vertexArray )[k] );
					k = ( k + 1 ) % vertexArray->size();
				}

				k = j;
				while( k != i )
				{
					polygonB->vertexArray->push_back( ( *vertexArray )[k] );
					k = ( k + 1 ) % vertexArray->size();
				}

				return true;
			}
		}
	}

	return false;
}

// This algorithm if far from perfect, and does not handle all desired cases.  It is an interesting problem.
// Limitations of this algorithm can sometimes be compensated for by pre-tesselating a polygon that would
// otherwise not get split properly by the given surface.
bool Polygon::SplitInTwoAgainstSurface( const Surface* surface, Polygon*& polygonA, Polygon*& polygonB, double minDistance, double maxDistance, double eps )
{
	polygonA = nullptr;
	polygonB = nullptr;

	IncreaseDensity( minDistance );

	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
	{
		int j = ( i + 1 ) % vertexArray->size();

		LineSegment lineSegment;
		lineSegment.vertex[0] = ( *vertexArray )[i];
		lineSegment.vertex[1] = ( *vertexArray )[j];

		Surface::Side side[2];
		side[0] = surface->GetSide( lineSegment.vertex[0], eps );
		side[1] = surface->GetSide( lineSegment.vertex[1], eps );

		if( ( side[0] == Surface::INSIDE && side[1] == Surface::OUTSIDE ) ||
			( side[1] == Surface::INSIDE && side[0] == Surface::OUTSIDE ) )
		{
			SurfacePoint* surfacePoint = surface->FindSingleIntersection( lineSegment );
			if( !surfacePoint )
				return false;
			else
			{
				Vector point;
				surfacePoint->GetLocation( point );
				vertexArray->insert( vertexArray->begin() + j, point );
				delete surfacePoint;
			}
		}
	}

	std::vector< int > intersectionArray;
	int insideCount = 0;
	int outsideCount = 0;

	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
	{
		Surface::Side side = surface->GetSide( ( *vertexArray )[i] );
		switch( side )
		{
			case Surface::NEITHER_SIDE:
			{
				int j = ( i > 0 ) ? ( i - 1 ) : ( ( signed )vertexArray->size() - 1 );
				int k = ( i + 1 ) % vertexArray->size();

				if( ( surface->GetSide( ( *vertexArray )[j] ) == Surface::INSIDE && surface->GetSide( ( *vertexArray )[k] ) == Surface::OUTSIDE ) ||
					( surface->GetSide( ( *vertexArray )[k] ) == Surface::INSIDE && surface->GetSide( ( *vertexArray )[j] ) == Surface::OUTSIDE ) )
				{
					intersectionArray.push_back(i);
				}

				break;
			}
			case Surface::INSIDE:
			{
				insideCount++;
				break;
			}
			case Surface::OUTSIDE:
			{
				outsideCount++;
				break;
			}
		}
	}

	if( insideCount == 0 || outsideCount == 0 || intersectionArray.size() < 2 )
		return false;

	if( !Tessellate() )	// TODO: Tessellation sometimes fails when it shouldn't.  Fix it.
		return false;

	_3DMath::Plane plane;
	if( !GetPlane( plane ) )
		return false;

	for( int i = 0; i < ( signed )intersectionArray.size(); i++ )
	{
		int j = ( i + 1 ) % intersectionArray.size();

		Vector pointA = ( *vertexArray )[ intersectionArray[i] ];
		Vector pointB = ( *vertexArray )[ intersectionArray[j] ];

		VectorArray surfacePointArray;
		bool pathFound = false;

		double distance = pointA.Distance( pointB );
		if( distance < eps )
		{
			surfacePointArray.push_back( pointA );
			pathFound = true;
		}
		else
		{
			SurfacePoint* surfacePointA = surface->GetNearestSurfacePoint( pointA );
			SurfacePoint* surfacePointB = surface->GetNearestSurfacePoint( pointB );
			SurfacePoint* surfacePointC = nullptr;

			pathFound = surface->FindDirectPath( surfacePointA, surfacePointB, surfacePointArray, maxDistance, &plane );
			if( !pathFound )
			{
				// This is a bit of a hack.
				Vector center;
				GetCenter( center );

				surfacePointC = surface->GetNearestSurfacePoint( center );
				pathFound = surface->FindDirectPath( surfacePointA, surfacePointC, surfacePointArray, maxDistance, &plane );
				if( pathFound )
				{
					surfacePointArray.pop_back();
					pathFound = surface->FindDirectPath( surfacePointC, surfacePointB, surfacePointArray, maxDistance, &plane );
				}
			}

			delete surfacePointA;
			delete surfacePointB;
			delete surfacePointC;

			if( surfacePointArray.size() == 0 )
				pathFound = false;
		}

		// Does the path along the surface and in the plane of the polygon
		// stay within the polygon's area through the whole path?
		if( pathFound )
		{
			for( int k = 0; k < ( signed )surfacePointArray.size(); k++ )
			{
				if( !ContainsPoint( surfacePointArray[k] ) )
				{
					pathFound = false;
					break;
				}
			}

#if 0	// Disable this for now, but this is a more accurate test.  The intersection routine has bugs in it.
			if( pathFound )
			{
				for( int k = 0; k < ( signed )surfacePointArray.size() - 1; k++ )
				{
					LineSegment lineSegment;
					lineSegment.vertex[0] = surfacePointArray[k];
					lineSegment.vertex[1] = surfacePointArray[ k + 1 ];
					
					VectorList intersectionList;
					if( GetIntersectionPoints( lineSegment, intersectionList ) )
					{
						pathFound = false;
						break;
					}
				}
			}
#endif
		}

		// Finally, if we get here, we're ready to split the polygon in two.
		if( pathFound )
		{
			polygonA = new Polygon();
			polygonB = new Polygon();

			int k = ( intersectionArray[i] + 1 ) % vertexArray->size();
			while( k != intersectionArray[j] )
			{
				polygonA->vertexArray->push_back( ( *vertexArray )[k] );
				k = ( k + 1 ) % vertexArray->size();
			}

			k = ( intersectionArray[j] + 1 ) % vertexArray->size();
			while( k != intersectionArray[i] )
			{
				polygonB->vertexArray->push_back( ( *vertexArray )[k] );
				k = ( k + 1 ) % vertexArray->size();
			}

			for( int k = 0; k < ( signed )surfacePointArray.size(); k++ )
			{
				int l = signed( surfacePointArray.size() ) - 1 - k;
				polygonA->vertexArray->push_back( surfacePointArray[l] );
				polygonB->vertexArray->push_back( surfacePointArray[k] );
			}

			return true;
		}
	}

	return false;
}

bool Polygon::GetIntersectionPoints( const LineSegment& lineSegment, VectorList& intersectionList ) const
{
	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
	{
		int j = ( i + 1 ) % vertexArray->size();

		LineSegment edge;
		edge.vertex[0] = ( *vertexArray )[i];
		edge.vertex[1] = ( *vertexArray )[j];

		Vector intersectionPoint;
		if( edge.IntersectsWith( lineSegment, &intersectionPoint ) )
			intersectionList.push_back( intersectionPoint );
	}

	return( intersectionList.size() > 0 ? true : false );
}

double Polygon::GetArea( void ) const
{
	double totalArea = 0.0;

	for( IndexTriangleList::const_iterator iter = indexTriangleList->cbegin(); iter != indexTriangleList->end(); iter++ )
	{
		const IndexTriangle& indexTriangle = *iter;
		Triangle triangle;
		indexTriangle.GetTriangle( triangle, vertexArray );
		totalArea += triangle.Area();
	}

	return totalArea;
}

bool Polygon::Tessellate( void ) const
{
	Plane plane;
	if( !GetPlane( plane ) )
		return false;

	indexTriangleList->clear();

	std::vector< int > indexArray;
	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
		indexArray.push_back(i);

	while( indexArray.size() > 2 )
	{
		int i;
		for( i = 0; i < ( signed )indexArray.size(); i++ )
		{
			IndexTriangle indexTriangle(
					indexArray[i],
					indexArray[ ( i + 1 ) % indexArray.size() ],
					indexArray[ ( i + 2 ) % indexArray.size() ] );

			Triangle triangle;
			indexTriangle.GetTriangle( triangle, vertexArray );

			Vector edge[2];
			edge[0].Subtract( triangle.vertex[1], triangle.vertex[0] );
			edge[1].Subtract( triangle.vertex[2], triangle.vertex[1] );

			Vector cross;
			cross.Cross( edge[0], edge[1] );
			double dot = cross.Dot( plane.normal );
			if( dot < 0.0 )
				continue;

			int j;
			for( j = 0; j < ( signed )indexArray.size(); j++ )
			{
				if( j == i || j == ( i + 1 ) % indexArray.size() || j == ( i + 2 ) % indexArray.size() )
					continue;
				if( triangle.ContainsPoint( ( *vertexArray )[ indexArray[j] ] ) )
					break;
			}

			if( j < ( signed )indexArray.size() )
				continue;

			indexTriangleList->push_back( indexTriangle );
			indexArray.erase( indexArray.begin() + ( i + 1 ) % indexArray.size() );
			break;
		}

		if( i == ( signed )indexArray.size() )
			return false;		// Avoid an infinite loop.
	}

	return true;
}

// Polygon.cpp