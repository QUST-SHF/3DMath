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
}

/*virtual*/ Polygon::~Polygon( void )
{
	delete vertexArray;
}

bool Polygon::SplitAgainstSurface( const Surface* surface, Polygon& insidePolygon, Polygon& outsidePolygon, double maxDistanceFromSurface ) const
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
}

void Polygon::Tessellate( IndexTriangleList& indexTriangleList ) const
{
	indexTriangleList.clear();

	VectorArray pointArray;
	for( int i = 0; i < ( signed )vertexArray->size(); i++ )
		pointArray.push_back( ( *vertexArray )[i] );

	while( pointArray.size() > 2 )
	{
		for( int i = 0; i < ( signed )vertexArray->size(); i++ )
		{
			IndexTriangle indexTriangle( i, ( i + 1 ) % vertexArray->size(), ( i + 2 ) % vertexArray->size() );

			Triangle triangle;
			indexTriangle.GetTriangle( triangle, &pointArray );

			int j;
			for( j = 0; j < ( signed )vertexArray->size(); j++ )
			{
				if( j == i )
					continue;

				if( triangle.ProperlyContainsPoint( ( *vertexArray )[j] ) )
					break;
			}

			if( j < ( signed )vertexArray->size() )
				continue;

			indexTriangleList.push_back( indexTriangle );
			pointArray.erase( pointArray.begin() + ( i + 1 ) % vertexArray->size() );
			break;
		}
	}
}

// Polygon.cpp