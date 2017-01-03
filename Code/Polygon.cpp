// Polygon.cpp

#include "Polygon.h"
#include "Triangle.h"
#include "IndexTriangle.h"
#include "LineSegment.h"
#include "Surface.h"
#include "AffineTransform.h"

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

void Polygon::GetIntegratedCenter( Vector& center, double delta ) const
{
	center.Set( 0.0, 0.0, 0.0 );
	int count = 0;

	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
	{
		int j = ( i + 1 ) % vertexArray->size();
		LineSegment lineSegment( ( *vertexArray )[i], ( *vertexArray )[j] );

		int stepCount = ( int )floor( lineSegment.Length() / delta );

		for( int k = 0; k < stepCount; k++ )
		{
			double lambda = double(k) / double( stepCount );

			Vector point;
			lineSegment.Lerp( lambda, point );

			center.Add( point );
			count++;
		}
	}

	center.Scale( 1.0 / double( count ) );
}

bool Polygon::SplitAgainstSurface( const Surface* surface, PolygonList& polygonList, double maxDistanceFromSurface ) const
{
	struct Node
	{
		Vector point;
		Surface::Side side;
	};

	std::vector< Node > nodeArray;

	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
	{
		Node node;
		node.point = ( *vertexArray )[i];
		node.side = surface->GetSide( node.point );
		nodeArray.push_back( node );
	}

	std::vector< int > intersectionArray;

	int i = 0;
	while( i < ( signed )nodeArray.size() )
	{
		int j = ( i + 1 ) % nodeArray.size();

		if( ( nodeArray[i].side == Surface::INSIDE && nodeArray[j].side == Surface::OUTSIDE ) ||
			( nodeArray[j].side == Surface::INSIDE && nodeArray[i].side == Surface::OUTSIDE ) )
		{
			LineSegment lineSegment;
			lineSegment.vertex[0] = nodeArray[i].point;
			lineSegment.vertex[1] = nodeArray[j].point;
			SurfacePoint* surfacePoint = surface->FindSingleIntersection( lineSegment );
			if( !surfacePoint )
				return false;

			Node node;
			surfacePoint->GetLocation( node.point );
			node.side = Surface::NEITHER_SIDE;
			if( j == 0 )
			{
				nodeArray.push_back( node );
				intersectionArray.push_back( nodeArray.size() - 1 );
			}
			else
			{
				std::vector< Node >::iterator iter( nodeArray.begin() + j );
				nodeArray.insert( iter, node );
				intersectionArray.push_back(j);
			}

			i += 2;

			delete surfacePoint;
		}
		else
		{
			int k = ( i > 0 ) ? ( i - 1 ) : ( nodeArray.size() - 1 );

			if( ( nodeArray[k].side == Surface::INSIDE && nodeArray[i].side == Surface::NEITHER_SIDE && nodeArray[j].side == Surface::OUTSIDE ) ||
				( nodeArray[j].side == Surface::INSIDE && nodeArray[i].side == Surface::NEITHER_SIDE && nodeArray[k].side == Surface::OUTSIDE ) )
			{
				intersectionArray.push_back(i);
			}

			i++;
		}
	}

	if( intersectionArray.size() < 2 )
		return false;

	Plane plane;
	if( !GetPlane( plane ) )
		return false;

	for( int i = 0; i < ( signed )intersectionArray.size(); i++ )
	{
		int j0 = intersectionArray[i];
		int j1 = intersectionArray[ ( i + 1 ) % intersectionArray.size() ];

		Polygon* polygon = new Polygon();
		polygonList.push_back( polygon );

		int k = ( j0 + 1 ) % nodeArray.size();
		while( k != j1 )
		{
			polygon->vertexArray->push_back( nodeArray[k].point );
			k = ( k + 1 ) % nodeArray.size();
		}

		SurfacePoint* surfacePointA = surface->GetNearestSurfacePoint( nodeArray[ j1 ].point );
		SurfacePoint* surfacePointB = surface->GetNearestSurfacePoint( nodeArray[ j0 ].point );

		bool pathFound = false;
		if( surfacePointA && surfacePointB )
			pathFound = surface->FindDirectPath( surfacePointA, surfacePointB, *polygon->vertexArray, maxDistanceFromSurface, &plane );

		delete surfacePointA;
		delete surfacePointB;

		if( !pathFound )
			return false;
	}

	return true;
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