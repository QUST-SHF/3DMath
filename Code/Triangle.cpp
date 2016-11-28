// Triangle.cpp

#include "Triangle.h"
#include "Plane.h"

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

void Triangle::GetNormal( Vector& normal, int index ) const
{
	Vector edge[2];
	edge[0].Subtract( vertex[ ( index + 1 ) % 3 ], vertex[ index ] );
	edge[1].Subtract( vertex[ ( index + 2 ) % 3 ], vertex[ index ] );
	normal.Cross( edge[0], edge[1] );
}

double Triangle::Area( void ) const
{
	Vector normal;
	GetNormal( normal, 0 );
	double area = normal.Length();
	return area;
}

void Triangle::GetPlane( Plane& plane ) const
{
	Vector normal;
	GetNormal( normal, 0 );
	plane.SetCenterAndNormal( vertex[0], normal );
}

bool Triangle::ContainsPoint( const Vector& point, double eps /*= EPSILON*/ ) const
{
	Plane plane;
	GetPlane( plane );

	if( plane.GetSide( point, eps ) != Plane::SIDE_NEITHER )
		return false;

	double totalArea = Area();
	double accumulatedArea = 0.0;
	for( int i = 0; i < 3; i++ )
		accumulatedArea = Triangle( vertex[i], vertex[ ( i + 1 ) % 3 ], point ).Area();

	return( fabs( totalArea - accumulatedArea ) < eps ? true : false );
}

bool Triangle::IsDegenerate( double eps /*= EPSILON*/ ) const
{
	if( Area() < eps )
		return true;
	return false;
}

bool Triangle::Intersect( const LineSegment& lineSegment, Vector& intersectionPoint ) const
{
	Plane plane;
	GetPlane( plane );

	if( !plane.Intersect( lineSegment, intersectionPoint ) )
		return false;

	return ContainsPoint( intersectionPoint );
}

// Triangle.cpp