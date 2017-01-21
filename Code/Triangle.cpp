// Triangle.cpp

#include "Triangle.h"
#include "LineSegment.h"
#include "Plane.h"
#include "Line.h"

using namespace _3DMath;

Triangle::Triangle( void )
{
}

Triangle::Triangle( const Vector& vertex0, const Vector& vertex1, const Vector& vertex2 )
{
	vertex[0] = vertex0;
	vertex[1] = vertex1;
	vertex[2] = vertex2;
}

/*virtual*/ Triangle::~Triangle( void )
{
}

void Triangle::GetNormal( Vector& normal ) const
{
	Vector edge[2];
	edge[0].Subtract( vertex[1], vertex[0] );
	edge[1].Subtract( vertex[2], vertex[0] );
	normal.Cross( edge[0], edge[1] );
}

void Triangle::GetCenter( Vector& center ) const
{
	center.Add( vertex[0], vertex[1] );
	center.Add( vertex[2] );
	center.Scale( 1.0 / 3.0 );
}

double Triangle::Area( void ) const
{
	Vector normal;
	GetNormal( normal );
	double area = normal.Length() / 2.0;
	return area;
}

void Triangle::GetPlane( Plane& plane ) const
{
	Vector normal;
	GetNormal( normal );
	plane.SetCenterAndNormal( vertex[0], normal );
}

bool Triangle::ContainsPoint( const Vector& point, double eps /*= EPSILON*/ ) const
{
	Plane plane;
	GetPlane( plane );

	if( plane.GetSide( point, eps ) != Plane::SIDE_NEITHER )
		return false;

	for( int i = 0; i < 3; i++ )
	{
		Vector edge, vec, cross;
		edge.Subtract( vertex[ ( i + 1 ) % 3 ], vertex[i] );
		vec.Subtract( point, vertex[i] );
		cross.Cross( edge, vec );
		double dot = cross.Dot( plane.normal );
		if( dot <= -eps )
			return false;
	}

	return true;
}

bool Triangle::ProperlyContainsPoint( const Vector& point, double eps /*= EPSILON*/ ) const
{
	Plane plane;
	GetPlane( plane );

	if( plane.GetSide( point, eps ) != Plane::SIDE_NEITHER )
		return false;

	for( int i = 0; i < 3; i++ )
	{
		Plane plane;
		Vector edge, planeNormal;
		edge.Subtract( vertex[ ( i + 1 ) % 3 ], vertex[i] );
		planeNormal.Cross( edge, plane.normal );
		plane.SetCenterAndNormal( vertex[i], planeNormal );
		if( Plane::SIDE_BACK != plane.GetSide( point, eps ) )
			return false;
	}

	return true;
}

bool Triangle::IsDegenerate( double eps /*= EPSILON*/ ) const
{
	if( Area() < eps )
		return true;
	return false;
}

bool Triangle::Intersect( const LineSegment& lineSegment, Vector& intersectionPoint, double eps /*= EPSILON*/ ) const
{
	Plane plane;
	GetPlane( plane );

	if( !plane.Intersect( lineSegment, intersectionPoint, eps ) )
		return false;

	return ContainsPoint( intersectionPoint, eps );
}

void Triangle::GetEdges( LineSegment* edges ) const
{
	for( int i = 0; i < 3; i++ )
	{
		int j = ( i + 1 ) % 3;
		edges[i].vertex[0] = vertex[i];
		edges[i].vertex[1] = vertex[j];
	}
}

double Triangle::DistanceToPoint( const Vector& point ) const
{
	Plane plane;
	GetPlane( plane );

	double distance = fabs( plane.Distance( point ) );
	Vector nearestPoint = point;
	plane.NearestPoint( nearestPoint );
	if( ContainsPoint( nearestPoint ) )
		return distance;

	LineSegment edges[3];
	GetEdges( edges );

	for( int i = 0; i < 3; i++ )
	{
		Line line( edges[i] );
		distance = line.ShortestDistance( point );
		if( edges[i].ContainsPoint( point ) )
			return distance;
	}

	double smallestDistance = 0.0;

	for( int i = 0; i < 3; i++ )
	{
		distance = vertex[i].Distance( point );
		if( distance < smallestDistance || smallestDistance == 0.0 )
			smallestDistance = distance;
	}

	return smallestDistance;
}

// Triangle.cpp