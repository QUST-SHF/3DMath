// Plane.cpp

#include "Plane.h"
#include "LineSegment.h"
#include "Line.h"
#include "AffineTransform.h"

using namespace _3DMath;

Plane::Plane( void )
{
	normal.Set( 0.0, 0.0, 1.0 );
	centerDotNormal = 0.0;
}

Plane::Plane( const Vector& center, const Vector& normal )
{
	SetCenterAndNormal( center, normal );
}

Plane::~Plane( void )
{
}

void Plane::SetUsingTriangle( const Triangle& triangle )
{
	triangle.GetPlane( *this );
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

bool Plane::Intersect( const Line& line, Vector& intersectionPoint, double eps /*= EPSILON*/ ) const
{
	double dot = line.normal.Dot( normal );
	if( fabs( dot ) < eps )
		return false;

	double lambda = ( centerDotNormal - line.center.Dot( normal ) ) / dot;
	intersectionPoint.AddScale( line.center, line.normal, lambda );
	return true;
}

bool Plane::Intersect( const LineSegment& lineSegment, Vector& intersectionPoint, double eps /*= EPSILON*/ ) const
{
	Line line( lineSegment );
	if( !Intersect( line, intersectionPoint, eps ) )
		return false;

	return lineSegment.ContainsPoint( intersectionPoint, eps );
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

		if( triangle.IsDegenerate() )
			continue;

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

void Plane::Reflect( Vector& point ) const
{
	//...
}

void Plane::Transform( const AffineTransform& affineTransform, const LinearTransform* normalTransform /*= nullptr*/ )
{
	Vector center;
	GetCenter( center );

	affineTransform.Transform( center );

	static LinearTransform normalTransformStorage;		// This makes us non-thread-safe, by the way.
	if( !normalTransform )
	{
		affineTransform.linearTransform.GetNormalTransform( normalTransformStorage );
		normalTransform = &normalTransformStorage;
	}

	normalTransform->Transform( normal );

	SetCenterAndNormal( center, normal );
}

bool Plane::IsEqualTo( const Plane& plane, double eps /*= EPSILION*/ ) const
{
	if( fabs( centerDotNormal - plane.centerDotNormal ) >= eps )
		return false;

	if( !normal.IsEqualTo( plane.normal, eps ) )
		return false;

	return true;
}

// Plane.cpp