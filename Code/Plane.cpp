// Plane.cpp

#include "Plane.h"
#include "LineSegment.h"
#include "AxisAlignedBox.h"

using namespace _3DMath;

Plane::Plane( void )
{
	normal.Set( 0.0, 0.0, 1.0 );
	centerDotNormal = 0.0;
}

Plane::~Plane( void )
{
}

void Plane::SetCenterAndNormal( const Vector& center, const Vector& normal )
{
	normal.GetNormalized( this->normal );
	centerDotNormal = center.Dot( this->normal );
}

void Plane::GetCenter( Vector& center ) const
{
	normal.GetScaled( center, centerDotNormal );
}

double Plane::Distance( const Vector& point ) const
{
	double distance = point.Dot( normal ) - centerDotNormal;
	return distance;
}

Plane::Side Plane::GetSide( const Vector& point, double eps /*= EPSILON*/ ) const
{
	double distance = Distance( point );
	if( distance > eps )
		return SIDE_FRONT;
	else if( distance < -eps )
		return SIDE_BACK;
	return SIDE_NEITHER;
}

void Plane::NearestPoint( Vector& point ) const
{
	double distance = Distance( point );
	point.AddScale( normal, -distance );
}

bool Plane::Intersect( const LineSegment& lineSegment, Vector& intersectionPoint, double eps /*= EPSILON*/ ) const
{
	return false;

	/*
	Line line( lineSegment );
	if( !Intersect( line, intersectionPoint ) )
		return false;

	double lambda = lineSegment.LerpInverse( intersectionPoint );
	return AxisAlignedBox.InInterval( 0.0, 1.0, lambda, eps );
	*/
}

bool Plane::SplitTriangle( const Triangle& triangle, TriangleList& frontList, TriangleList& backList ) const
{
	VectorArray vertexArray;

	int q = -1;

	for( int i = 0; i < 3; i++ )
	{
		vertexArray.push_back( triangle.vertex[i] );

		int j = ( i + 1 ) % 3;
		LineSegment edge( triangle.vertex[i], triangle.vertex[j] );
		Vector intersectionPoint;
		if( Intersect( edge, intersectionPoint ) )
		{
			int k;
			for( k = 0; k < ( signed )vertexArray.size(); k++ )
				if( vertexArray[k].IsEqualTo( intersectionPoint ) )
					break;

			if( k == vertexArray.size() )
			{
				vertexArray.push_back( intersectionPoint );
				if( q < 0 )
					q = ( signed )vertexArray.size() - 1;
			}
		}
	}

	if( vertexArray.size() == 3 )
		return false;

	for( int i = 0; i < ( signed )vertexArray.size() - 2; i++ )
	{
		Triangle triangle;
		triangle.vertex[0] = vertexArray[q];
		triangle.vertex[1] = vertexArray[ ( q + i + 1 ) % vertexArray.size() ];
		triangle.vertex[2] = vertexArray[ ( q + i + 2 ) % vertexArray.size() ];

		int k;
		for( k = 0; k < 3; k++ )
		{
			Side side = GetSide( triangle.vertex[k] );
			if( side == SIDE_FRONT )
			{
				frontList.push_back( triangle );
				break;
			}
			else if( side == SIDE_BACK )
			{
				backList.push_back( triangle );
				break;
			}
		}

		if( k == 3 )
			return false;		// Something went wrong!
	}

	return true;
}

// Plane.cpp