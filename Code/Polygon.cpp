// Polygon.cpp

#include "Polygon.h"
#include "Triangle.h"
#include "IndexTriangle.h"
#include "LineSegment.h"
#include "Surface.h"

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

bool Polygon::GetPlane( Plane& plane ) const
{
	if( vertexArray->size() < 3 )
		return false;

	Vector normal, center;
	normal.Set( 0.0, 0.0, 0.0 );
	center.Set( 0.0, 0.0, 0.0 );

	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
	{
		// This is the Newel method.
		int j = ( i + 1 ) % vertexArray->size();
		const Vector& pointA = ( *vertexArray )[i];
		const Vector& pointB = ( *vertexArray )[j];
		normal.x += ( pointA.y - pointB.y ) * ( pointA.z + pointB.z );
		normal.y += ( pointA.z - pointB.z ) * ( pointA.x + pointB.x );
		normal.z += ( pointA.x - pointB.x ) * ( pointA.y + pointB.y );
		center.Add( pointA );
	}

	center.Scale( 1.0 / double( vertexArray->size() ) );
	plane.SetCenterAndNormal( center, normal );

	return true;
}

bool Polygon::SplitAgainstSurface( const Surface* surface, PolygonList& polygonList, double maxDistanceFromSurface ) const
{
	return false;

	/*
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

	for( int i = 0; i < ( signed )nodeArray.size(); i++ )
	{
		int j = ( i + 1 ) % nodeArray.size();

		if( ( nodeArray[i].side == Surface::INSIDE && nodeArray[j].side == Surface::OUTSIDE ) ||
			( nodeArray[i].side == Surface::OUTSIDE && nodeArray[j].side == Surface::INSIDE ) )
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
			std::vector< Node >::iterator iter( nodeArray.begin() + j );
			nodeArray.insert( iter, node );
		}
	}

	for( int i = 0; i < 2; i++ )
	{
		Polygon* polygon = nullptr;
		Surface::Side currentSide, otherSide;

		if( i == 0 )
		{
			currentSide = Surface::INSIDE;
			otherSide = Surface::OUTSIDE;
			polygon = &insidePolygon;
		}
		else
		{
			currentSide = Surface::OUTSIDE;
			otherSide = Surface::INSIDE;
			polygon = &outsidePolygon;
		}

		int j;
		for( j = 0; j < ( signed )nodeArray.size(); j++ )
			if( nodeArray[j].side == currentSide )
				break;

		if( j == ( signed )nodeArray.size() )
			return false;

		int k = j;

		do
		{
			int l = ( k + 1 ) % nodeArray.size();

			if( nodeArray[k].side == Surface::NEITHER_SIDE && nodeArray[l].side == otherSide )
			{
				while( nodeArray[l].side != Surface::NEITHER_SIDE )
					l = ( l + 1 ) % nodeArray.size();
				
				SurfacePoint* surfacePointA = surface->GetNearestSurfacePoint( nodeArray[k].point );
				SurfacePoint* surfacePointB = surface->GetNearestSurfacePoint( nodeArray[l].point );

				surface->FindDirectPath( surfacePointA, surfacePointB, *polygon->vertexArray, maxDistanceFromSurface );

				l = ( l + 1 ) % nodeArray.size();
			}
			else
			{
				polygon->vertexArray->push_back( nodeArray[k].point );
			}

			k = l;
		}
		while( k != j );
	}

	return true;
	*/
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
		for( int i = 0; i < ( signed )indexArray.size(); i++ )
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
				if( triangle.ProperlyContainsPoint( ( *vertexArray )[ indexArray[j] ] ) )
					break;

			if( j < ( signed )indexArray.size() )
				continue;

			indexTriangleList->push_back( indexTriangle );
			indexArray.erase( indexArray.begin() + ( i + 1 ) % indexArray.size() );
			break;
		}
	}

	return true;
}

// Polygon.cpp