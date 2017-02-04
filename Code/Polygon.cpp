// Polygon.cpp

#include "Polygon.h"
#include "Triangle.h"
#include "IndexTriangle.h"
#include "LineSegment.h"
#include "Surface.h"
#include "AffineTransform.h"
#include "Graph.h"
#include "Exception.h"

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

// Finding a solution to the general case of splitting a polygon against a surface is
// a large and complex problem.  I have wracked my brain about it for quite some time.
// What I would do is convert the polygon into a planar graph, then generate another
// planar graph from the polygon's plane intersecting the surface.  I would then combine
// the two planar graphs into one by creating intersections where necessary, then generate
// polygons from the appropriate holes in the composite graph.  Each sub-problem here
// is no small task.  Given enough time, and with enough effort, I'm sure I could pull it off.
// Here, however, in the interest of time and simplicity, I'm going to solve only a very common
// special-, yet somewhat general-case of the problem that arrises when the surfaces weaves
// in and out of the polygon zero or more times.
bool Polygon::SplitAgainstSurface( const Surface* surface, PolygonList& polygonList, double minDistance, double maxDistance, double eps /*= EPSILON*/ ) const
{
	Polygon polygon;
	GetCopy( polygon );

	polygon.IncreaseDensity( minDistance );

	for( int i = 0; i < ( signed )polygon.vertexArray->size(); i++ )
	{
		int j = ( i + 1 ) % polygon.vertexArray->size();

		LineSegment lineSegment;
		lineSegment.vertex[0] = ( *polygon.vertexArray )[i];
		lineSegment.vertex[1] = ( *polygon.vertexArray )[j];

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
				polygon.vertexArray->insert( polygon.vertexArray->begin() + j, point );
				delete surfacePoint;
			}
		}
	}

	std::vector< int > intersectionArray;
	int insideCount = 0;
	int outsideCount = 0;

	for( int i = 0; i < ( signed )polygon.vertexArray->size(); i++ )
	{
		Surface::Side side = surface->GetSide( ( *polygon.vertexArray )[i] );
		switch( side )
		{
			case Surface::NEITHER_SIDE:
			{
				int j = ( i > 0 ) ? ( i - 1 ) : ( ( signed )polygon.vertexArray->size() - 1 );
				int k = ( i + 1 ) % polygon.vertexArray->size();

				if( ( surface->GetSide( ( *polygon.vertexArray )[j] ) == Surface::INSIDE && surface->GetSide( ( *polygon.vertexArray )[k] ) == Surface::OUTSIDE ) ||
					( surface->GetSide( ( *polygon.vertexArray )[k] ) == Surface::INSIDE && surface->GetSide( ( *polygon.vertexArray )[j] ) == Surface::OUTSIDE ) )
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

	Plane plane;
	polygon.GetPlane( plane );

	for( int i = 0; i < ( signed )intersectionArray.size(); i++ )
	{
		Polygon* newPolygon = new Polygon();
		polygonList.push_back( newPolygon );

		int j = ( intersectionArray[i] + 1 ) % polygon.vertexArray->size();

		while( j != intersectionArray[ ( i + 1 ) % intersectionArray.size() ] )
		{
			newPolygon->vertexArray->push_back( ( *polygon.vertexArray )[j] );
			j = ( j + 1 ) % polygon.vertexArray->size();
		}

		int k = intersectionArray[i];

		Vector pointA = ( *polygon.vertexArray )[j];
		Vector pointB = ( *polygon.vertexArray )[k];

		double distance = pointA.Distance( pointB );
		if( distance < eps )
			newPolygon->vertexArray->push_back( pointA );
		else
		{
			SurfacePoint* surfacePointA = surface->GetNearestSurfacePoint( pointA );
			SurfacePoint* surfacePointB = surface->GetNearestSurfacePoint( pointB );
			SurfacePoint* surfacePointC = nullptr;

			bool pathFound = surface->FindDirectPath( surfacePointA, surfacePointB, *newPolygon->vertexArray, maxDistance, &plane );
			if( !pathFound )
			{
				// This is a bit of a hack.
				Vector center;
				GetCenter( center );

				surfacePointC = surface->GetNearestSurfacePoint( center );
				pathFound = surface->FindDirectPath( surfacePointA, surfacePointC, *newPolygon->vertexArray, maxDistance, &plane );
				if( pathFound )
				{
					newPolygon->vertexArray->pop_back();
					pathFound = surface->FindDirectPath( surfacePointC, surfacePointB, *newPolygon->vertexArray, maxDistance, &plane );
				}
			}

			delete surfacePointA;
			delete surfacePointB;
			delete surfacePointC;

			if( !pathFound )
				return false;
		}

		if( newPolygon->vertexArray->size() <= 2 )
			return false;
	}

	return true;
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