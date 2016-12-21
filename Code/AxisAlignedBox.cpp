// AxisAlignedBox.cpp

#include "AxisAlignedBox.h"
#include "Triangle.h"
#include "LineSegment.h"
#include "Line.h"
#include "Plane.h"
#include "Renderer.h"

using namespace _3DMath;

AxisAlignedBox::AxisAlignedBox( void )
{
	negCorner.Set( 0.0, 0.0, 0.0 );
	posCorner.Set( 0.0, 0.0, 0.0 );
}

AxisAlignedBox::AxisAlignedBox( const AxisAlignedBox& box )
{
	negCorner = box.negCorner;
	posCorner = box.posCorner;
}

AxisAlignedBox::AxisAlignedBox( const Vector& negCorner, const Vector& posCorner )
{
	this->negCorner = negCorner;
	this->posCorner = posCorner;
}

AxisAlignedBox::~AxisAlignedBox( void )
{
}

void AxisAlignedBox::GrowToIncludePoint( const Vector& point )
{
	ExpandInterval( negCorner.x, posCorner.x, point.x );
	ExpandInterval( negCorner.y, posCorner.y, point.y );
	ExpandInterval( negCorner.z, posCorner.z, point.z );
}

void AxisAlignedBox::SetCenterAndDimensions( const Vector& center, const Vector& dimensions )
{
	negCorner.AddScale( center, 1.0, dimensions, -0.5 );
	posCorner.AddScale( center, 1.0, dimensions, 0.5 );
}

/*static*/ void AxisAlignedBox::ExpandInterval( double& min, double& max, double value )
{
	if( value < min )
		min = value;
	if( value > max )
		max = value;
}

bool AxisAlignedBox::Intersect( const AxisAlignedBox& boxA, const AxisAlignedBox& boxB )
{
	return false;
}

void AxisAlignedBox::Combine( const AxisAlignedBox& boxA, const AxisAlignedBox& boxB )
{
	negCorner.Min( boxA.negCorner, boxB.negCorner );
	posCorner.Max( boxA.posCorner, boxA.posCorner );
}

void AxisAlignedBox::GetCenter( Vector& center ) const
{
	center.Lerp( negCorner, posCorner, 0.5 );
}

void AxisAlignedBox::SplitInTwo( AxisAlignedBox& boxA, AxisAlignedBox& boxB, Plane* plane /*= nullptr*/, int split /*= -1*/ ) const
{
	if( split == -1 )
	{
		Vector vector;
		vector.Subtract( posCorner, negCorner );

		if( vector.x > vector.y )
		{
			if( vector.x > vector.z )
				split = 0;
			else
				split = 2;
		}
		else
		{
			if( vector.y > vector.z )
				split = 1;
			else
				split = 2;
		}
	}

	Vector center;
	GetCenter( center );

	boxA = *this;
	boxB = *this;

	switch( split )
	{
		case 0:		// Split along X.
		{
			boxA.posCorner.x = center.x;
			boxB.negCorner.x = center.x;

			if( plane )
				plane->SetCenterAndNormal( center, Vector( 1.0, 0.0, 0.0 ) );

			break;
		}
		case 1:		// Split along Y.
		{
			boxA.posCorner.y = center.y;
			boxB.negCorner.y = center.y;

			if( plane )
				plane->SetCenterAndNormal( center, Vector( 0.0, 1.0, 0.0 ) );

			break;
		}
		case 2:		// Split along Z.
		{
			boxA.posCorner.z = center.z;
			boxB.negCorner.z = center.z;

			if( plane )
				plane->SetCenterAndNormal( center, Vector( 0.0, 0.0, 1.0 ) );

			break;
		}
	}
}

void AxisAlignedBox::Render( Renderer& renderer ) const
{
	renderer.BeginDrawMode( Renderer::DRAW_MODE_LINES );

	renderer.IssueVertex( Vertex( Vector( negCorner.x, negCorner.y, negCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( posCorner.x, negCorner.y, negCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( negCorner.x, posCorner.y, negCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( posCorner.x, posCorner.y, negCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( negCorner.x, negCorner.y, posCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( posCorner.x, negCorner.y, posCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( negCorner.x, posCorner.y, posCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( posCorner.x, posCorner.y, posCorner.z ) ) );

	renderer.IssueVertex( Vertex( Vector( negCorner.x, negCorner.y, negCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( negCorner.x, posCorner.y, negCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( posCorner.x, negCorner.y, negCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( posCorner.x, posCorner.y, negCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( negCorner.x, negCorner.y, posCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( negCorner.x, posCorner.y, posCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( posCorner.x, negCorner.y, posCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( posCorner.x, posCorner.y, posCorner.z ) ) );

	renderer.IssueVertex( Vertex( Vector( negCorner.x, negCorner.y, negCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( negCorner.x, negCorner.y, posCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( posCorner.x, negCorner.y, negCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( posCorner.x, negCorner.y, posCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( negCorner.x, posCorner.y, negCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( negCorner.x, posCorner.y, posCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( posCorner.x, posCorner.y, negCorner.z ) ) );
	renderer.IssueVertex( Vertex( Vector( posCorner.x, posCorner.y, posCorner.z ) ) );

	renderer.EndDrawMode();
}

/*static*/ bool AxisAlignedBox::InInterval( double min, double max, double value, double eps /*= EPSILON*/ )
{
	if( min - eps <= value && value <= max + eps )
		return true;
	return false;
}

bool AxisAlignedBox::ContainsPoint( const Vector& point, double eps /*= EPSILON*/ ) const
{
	if( !InInterval( negCorner.x, posCorner.x, point.x, eps ) )
		return false;
	if( !InInterval( negCorner.y, posCorner.y, point.y, eps ) )
		return false;
	if( !InInterval( negCorner.z, posCorner.z, point.z, eps ) )
		return false;
	return true;
}

bool AxisAlignedBox::ContainsTriangle( const Triangle& triangle, double eps /*= EPSILON*/ ) const
{
	for( int i = 0; i < 3; i++ )
		if( !ContainsPoint( triangle.vertex[i], eps ) )
			return false;
	return true;
}

bool AxisAlignedBox::ContainsLineSegment( const LineSegment& lineSegment, double eps /*= EPSILON*/ ) const
{
	for( int i = 0; i < 2; i++ )
		if( !ContainsPoint( lineSegment.vertex[i], eps ) )
			return false;
	return true;
}

bool AxisAlignedBox::IntersectsWithLineSegment( const LineSegment& lineSegment, double eps /*= EPSILON*/ ) const
{
	for( int i = 0; i < 2; i++ )
		if( ContainsPoint( lineSegment.vertex[i], eps ) )
			return true;

	Line line( lineSegment );

	for( int i = 0; i < 6; i++ )
	{
		Plane plane;

		switch(i)
		{
			case 0:
			{
				plane.SetCenterAndNormal( Vector( negCorner ), Vector( -1.0, 0.0, 0.0 ) );
				break;
			}
			case 1:
			{
				plane.SetCenterAndNormal( Vector( negCorner ), Vector( 0.0, -1.0, 0.0 ) );
				break;
			}
			case 2:
			{
				plane.SetCenterAndNormal( Vector( negCorner ), Vector( 0.0, 0.0, -1.0 ) );
				break;
			}
			case 3:
			{
				plane.SetCenterAndNormal( Vector( posCorner ), Vector( 1.0, 0.0, 0.0 ) );
				break;
			}
			case 4:
			{
				plane.SetCenterAndNormal( Vector( posCorner ), Vector( 0.0, 1.0, 0.0 ) );
				break;
			}
			case 5:
			{
				plane.SetCenterAndNormal( Vector( posCorner ), Vector( 0.0, 0.0, 1.0 ) );
				break;
			}
		}

		Vector intersectionPoint;
		if( plane.Intersect( line, intersectionPoint, eps ) && ContainsPoint( intersectionPoint, eps ) )
		{
			if( lineSegment.ContainsPoint( intersectionPoint, eps ) )
				return true;
		}
	}

	return false;
}

// AxisAlignedBox.cpp