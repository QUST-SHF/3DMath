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

// Finding a solution to the general case of splitting a polygon against a surface is
// a large and complex problem.  I have wracked my brain about it for quite some time.
// What I would do is convert the polygon into a planar graph, then generate another
// planar graph from the polygon's plane intersecting the surface.  I would then combine
// the two planar graphs into one by creating intersections where necessary, then I would
// generate polygons from the appropriate holes in the composite graph.  Each sub-problem here
// is no small task.  I'm sure I could do it given enough time, but here I'm going to solve only
// a very common special case of the problem that arrises when the split would result in exactly two
// polygons.  If no split would occur, or more than one split would occur, the routine fails.
bool Polygon::SplitAgainstSurface( const Surface* surface, Polygon*& insidePolygon, Polygon*& outsidePolygon, double minDistance, double maxDistance ) const
{
	// TODO: Write it here.
	return false;
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